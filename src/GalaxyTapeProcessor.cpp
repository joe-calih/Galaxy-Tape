#include "GalaxyTapeProcessor.h"
#include "GalaxyTapeEditor.h"

namespace
{
constexpr float maximumDelaySeconds = 2.0f;

juce::AudioParameterFloatAttributes linearAttributes(const juce::String& label)
{
    return juce::AudioParameterFloatAttributes().withLabel(label);
}
}

GalaxyTapeAudioProcessor::GalaxyTapeAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "GALAXY_TAPE_STATE", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
GalaxyTapeAudioProcessor::createParameterLayout()
{
    using Float = juce::AudioParameterFloat;
    using Bool = juce::AudioParameterBool;
    using Choice = juce::AudioParameterChoice;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<Float>("input_vol", "Input Vol",
                                       juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f),
                                       1.0f, linearAttributes("x")));
    layout.add(std::make_unique<Float>("head_select", "Head Select",
                                       juce::NormalisableRange<float>(0.0f, 7.0f, 1.0f),
                                       4.0f));
    layout.add(std::make_unique<Float>("echo_rate", "Echo Rate",
                                       juce::NormalisableRange<float>(40.0f, 1600.0f, 1.0f, 0.4f),
                                       360.0f, linearAttributes("ms")));
    layout.add(std::make_unique<Float>("treble", "Treble",
                                       juce::NormalisableRange<float>(-18.0f, 12.0f, 0.1f),
                                       0.0f, linearAttributes("dB")));
    layout.add(std::make_unique<Float>("echo_pan", "Echo Pan",
                                       juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
                                       0.0f));
    layout.add(std::make_unique<Float>("reverb_pan", "Reverb Pan",
                                       juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
                                       0.0f));
    layout.add(std::make_unique<Float>("feedback", "Feedback",
                                       juce::NormalisableRange<float>(0.0f, 0.95f, 0.001f),
                                       0.28f));
    layout.add(std::make_unique<Float>("bass", "Bass",
                                       juce::NormalisableRange<float>(-18.0f, 12.0f, 0.1f),
                                       0.0f, linearAttributes("dB")));
    layout.add(std::make_unique<Float>("echo_vol", "Echo Vol",
                                       juce::NormalisableRange<float>(0.0f, 1.5f, 0.001f),
                                       0.78f, linearAttributes("x")));
    layout.add(std::make_unique<Float>("reverb_vol", "Reverb Vol",
                                       juce::NormalisableRange<float>(0.0f, 1.5f, 0.001f),
                                       0.48f, linearAttributes("x")));
    layout.add(std::make_unique<Float>("output_vol", "Output Vol",
                                       juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f),
                                       1.0f, linearAttributes("x")));
    layout.add(std::make_unique<Bool>("input_send", "Input Send", true));
    layout.add(std::make_unique<Bool>("head_1", "Head 1", true));
    layout.add(std::make_unique<Bool>("head_2", "Head 2", true));
    layout.add(std::make_unique<Bool>("head_3", "Head 3", true));
    layout.add(std::make_unique<Bool>("tempo_sync", "Tempo Sync", false));
    layout.add(std::make_unique<Bool>("tape_loop", "Tape Loop", false));
    layout.add(std::make_unique<Choice>("tape_mode", "Tape Mode",
                                        juce::StringArray{"New", "Used", "Old"}, 0));
    layout.add(std::make_unique<Bool>("wet_solo", "Wet Solo", false));
    layout.add(std::make_unique<Bool>("power", "Power", true));
    return layout;
}

void GalaxyTapeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    const auto length = static_cast<int>(std::ceil(maximumDelaySeconds * sampleRate))
                        + juce::jmax(samplesPerBlock, 1) + 4;
    for (auto& channel : delayBuffer)
        channel.assign(static_cast<size_t>(length), 0.0f);
    reverbBuffer.setSize(2, juce::jmax(samplesPerBlock, 1));
    reverbBuffer.clear();
    lowBandState.fill(0.0f);
    highBandState.fill(0.0f);
    writePosition = 0;
    tapePhase = 0.0;
    reverb.reset();
}

void GalaxyTapeAudioProcessor::releaseResources()
{
    for (auto& channel : delayBuffer)
        channel.clear();
    reverbBuffer.setSize(0, 0);
    reverb.reset();
}

bool GalaxyTapeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();
    return input == output && (output == juce::AudioChannelSet::mono()
                               || output == juce::AudioChannelSet::stereo());
}

float GalaxyTapeAudioProcessor::readParameter(
    const juce::AudioProcessorValueTreeState& state, const juce::String& id) noexcept
{
    if (const auto* value = state.getRawParameterValue(id))
        return value->load(std::memory_order_relaxed);
    return 0.0f;
}

float GalaxyTapeAudioProcessor::readDelaySample(int channel, float delaySamples) const noexcept
{
    const auto& line = delayBuffer[static_cast<size_t>(channel)];
    if (line.empty())
        return 0.0f;

    const float position = static_cast<float>(writePosition) - delaySamples;
    const int lowerUnwrapped = static_cast<int>(std::floor(position));
    const float fraction = position - static_cast<float>(lowerUnwrapped);
    const int length = static_cast<int>(line.size());
    const int lower = (lowerUnwrapped % length + length) % length;
    const int upper = (lower + 1) % length;
    return line[static_cast<size_t>(lower)] * (1.0f - fraction)
           + line[static_cast<size_t>(upper)] * fraction;
}

void GalaxyTapeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin(buffer.getNumChannels(), 2);
    const int samples = buffer.getNumSamples();
    if (channels == 0 || samples == 0 || delayBuffer[0].empty())
        return;

    if (reverbBuffer.getNumSamples() < samples)
        reverbBuffer.setSize(2, samples, false, false, true);
    reverbBuffer.clear(0, samples);
    reverbBuffer.clear(1, samples);

    float inputLevel = 0.0f;
    for (int channel = 0; channel < channels; ++channel)
        inputLevel = juce::jmax(inputLevel, buffer.getMagnitude(channel, 0, samples));
    inputPeak.store(inputLevel, std::memory_order_relaxed);

    if (readParameter(parameters, "power") < 0.5f)
    {
        float outputLevel = 0.0f;
        for (int channel = 0; channel < channels; ++channel)
            outputLevel = juce::jmax(outputLevel, buffer.getMagnitude(channel, 0, samples));
        outputPeak.store(outputLevel, std::memory_order_relaxed);
        return;
    }

    const float inputGain = readParameter(parameters, "input_vol");
    const float outputGain = readParameter(parameters, "output_vol");
    const float baseRateMs = readParameter(parameters, "echo_rate");
    const float feedback = readParameter(parameters, "feedback");
    const float echoGain = readParameter(parameters, "echo_vol");
    const float reverbGain = readParameter(parameters, "reverb_vol");
    const float bassGain = juce::Decibels::decibelsToGain(readParameter(parameters, "bass"));
    const float trebleGain = juce::Decibels::decibelsToGain(readParameter(parameters, "treble"));
    const float echoPan = readParameter(parameters, "echo_pan");
    const float reverbPan = readParameter(parameters, "reverb_pan");
    const int headSelection = juce::jlimit(0, 7,
        juce::roundToInt(readParameter(parameters, "head_select")));
    const int tapeMode = juce::jlimit(0, 2,
        juce::roundToInt(readParameter(parameters, "tape_mode")));
    const bool inputSend = readParameter(parameters, "input_send") >= 0.5f;
    const bool tapeLoop = readParameter(parameters, "tape_loop") >= 0.5f;
    const bool tempoSync = readParameter(parameters, "tempo_sync") >= 0.5f;
    const bool wetSolo = readParameter(parameters, "wet_solo") >= 0.5f;

    bool heads[3] = {
        readParameter(parameters, "head_1") >= 0.5f,
        readParameter(parameters, "head_2") >= 0.5f,
        readParameter(parameters, "head_3") >= 0.5f
    };
    const float ratios[3] = {1.0f, 0.6666667f, 0.5f};
    const int selectedHeads = ((headSelection & 1) != 0 && heads[0] ? 1 : 0)
                              + ((headSelection & 2) != 0 && heads[1] ? 1 : 0)
                              + ((headSelection & 4) != 0 && heads[2] ? 1 : 0);

    float delayMs = baseRateMs;
    if (tempoSync)
    {
        double bpm = 120.0;
        if (auto* playHead = getPlayHead())
            if (const auto position = playHead->getPosition())
                if (const auto hostBpm = position->getBpm())
                    bpm = juce::jlimit(20.0, 400.0, *hostBpm);

        const std::array<double, 5> beatDivisions{0.25, 0.5, 1.0, 2.0, 4.0};
        const double desiredBeats = static_cast<double>(baseRateMs) * bpm / 60000.0;
        double nearest = beatDivisions.front();
        for (const auto beats : beatDivisions)
            if (std::abs(beats - desiredBeats) < std::abs(nearest - desiredBeats))
                nearest = beats;
        delayMs = static_cast<float>(nearest * 60000.0 / bpm);
    }

    const float tapeSaturation[3] = {1.05f, 1.35f, 1.8f};
    const float highFrequencyLoss[3] = {0.02f, 0.12f, 0.28f};
    const float wowDepthMs[3] = {0.015f, 0.09f, 0.22f};
    juce::Reverb::Parameters reverbParameters;
    reverbParameters.roomSize = 0.78f;
    reverbParameters.damping = juce::jlimit(0.15f, 0.95f, 0.45f + highFrequencyLoss[tapeMode]);
    reverbParameters.wetLevel = 1.0f;
    reverbParameters.dryLevel = 0.0f;
    reverbParameters.width = 1.0f;
    reverbParameters.freezeMode = 0.0f;
    reverb.setParameters(reverbParameters);

    const float echoLeftPan = std::sqrt(0.5f * (1.0f - echoPan));
    const float echoRightPan = std::sqrt(0.5f * (1.0f + echoPan));
    const float reverbLeftPan = std::sqrt(0.5f * (1.0f - reverbPan));
    const float reverbRightPan = std::sqrt(0.5f * (1.0f + reverbPan));
    float outputLevel = 0.0f;

    for (int sample = 0; sample < samples; ++sample)
    {
        float input[2]{};
        for (int channel = 0; channel < channels; ++channel)
            input[channel] = buffer.getSample(channel, sample) * inputGain;
        if (channels == 1)
            input[1] = input[0];

        const float wowMs = wowDepthMs[tapeMode]
                            * std::sin(static_cast<float>(tapePhase));
        float echoes[2]{};
        float feedbackSum[2]{};

        for (int channel = 0; channel < 2; ++channel)
        {
            for (int head = 0; head < 3; ++head)
            {
                if ((headSelection & (1 << head)) == 0 || !heads[head])
                    continue;

                const float delaySamples = juce::jlimit(1.0f,
                    static_cast<float>(delayBuffer[static_cast<size_t>(channel)].size() - 2),
                    static_cast<float>((delayMs * ratios[head] + wowMs)
                                       * 0.001 * currentSampleRate));
                const float tap = readDelaySample(channel, delaySamples);
                echoes[channel] += tap;
                feedbackSum[channel] += tap;
            }

            if (selectedHeads > 0)
                echoes[channel] /= static_cast<float>(selectedHeads);

            lowBandState[static_cast<size_t>(channel)] += 0.025f
                * (echoes[channel] - lowBandState[static_cast<size_t>(channel)]);
            highBandState[static_cast<size_t>(channel)] += 0.38f
                * (echoes[channel] - highBandState[static_cast<size_t>(channel)]);
            const float highBand = echoes[channel] - highBandState[static_cast<size_t>(channel)];
            float equalized = lowBandState[static_cast<size_t>(channel)] * bassGain
                              + (echoes[channel] - lowBandState[static_cast<size_t>(channel)]
                                 - highBand) + highBand * trebleGain;
            equalized *= 1.0f - highFrequencyLoss[tapeMode] * 0.35f;
            echoes[channel] = std::tanh(equalized * tapeSaturation[tapeMode]);

            const float incoming = inputSend && !tapeLoop ? input[channel] : 0.0f;
            const float loopFeedback = tapeLoop ? 0.995f : feedback;
            const float recursive = selectedHeads > 0
                ? (feedbackSum[channel] / static_cast<float>(selectedHeads)) * loopFeedback
                : 0.0f;
            delayBuffer[static_cast<size_t>(channel)][static_cast<size_t>(writePosition)]
                = std::tanh((incoming + recursive) * tapeSaturation[tapeMode]);
        }

        const float reverbInputLeft = selectedHeads == 0
            ? (inputSend && !tapeLoop ? input[0] : 0.0f) : echoes[0];
        const float reverbInputRight = selectedHeads == 0
            ? (inputSend && !tapeLoop ? input[1] : 0.0f) : echoes[1];
        reverbBuffer.setSample(0, sample, reverbInputLeft);
        reverbBuffer.setSample(1, sample, reverbInputRight);

        const float dryLeft = wetSolo ? 0.0f : input[0];
        const float dryRight = wetSolo ? 0.0f : input[1];
        if (channels == 1)
        {
            const float mono = dryLeft + echoes[0] * echoGain * 0.7071068f;
            buffer.setSample(0, sample, mono);
        }
        else
        {
            buffer.setSample(0, sample, dryLeft + echoes[0] * echoGain * echoLeftPan);
            buffer.setSample(1, sample, dryRight + echoes[1] * echoGain * echoRightPan);
        }

        writePosition = (writePosition + 1)
                        % static_cast<int>(delayBuffer[0].size());
        tapePhase += juce::MathConstants<double>::twoPi * 0.31 / currentSampleRate;
        if (tapePhase >= juce::MathConstants<double>::twoPi)
            tapePhase -= juce::MathConstants<double>::twoPi;
    }

    if (channels == 1)
    {
        reverb.processMono(reverbBuffer.getWritePointer(0), samples);
        for (int sample = 0; sample < samples; ++sample)
            buffer.addSample(0, sample, reverbBuffer.getSample(0, sample) * reverbGain);
    }
    else
    {
        reverb.processStereo(reverbBuffer.getWritePointer(0),
                             reverbBuffer.getWritePointer(1), samples);
        for (int sample = 0; sample < samples; ++sample)
        {
            buffer.addSample(0, sample, reverbBuffer.getSample(0, sample)
                                             * reverbGain * reverbLeftPan);
            buffer.addSample(1, sample, reverbBuffer.getSample(1, sample)
                                             * reverbGain * reverbRightPan);
        }
    }

    for (int channel = 0; channel < channels; ++channel)
    {
        buffer.applyGain(channel, 0, samples, outputGain);
        outputLevel = juce::jmax(outputLevel, buffer.getMagnitude(channel, 0, samples));
    }
    outputPeak.store(outputLevel, std::memory_order_relaxed);
}

void GalaxyTapeAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>&,
                                                     juce::MidiBuffer&)
{
}

juce::AudioProcessorEditor* GalaxyTapeAudioProcessor::createEditor()
{
    return new GalaxyTapeAudioProcessorEditor(*this);
}

void GalaxyTapeAudioProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    const auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destination);
}

void GalaxyTapeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GalaxyTapeAudioProcessor();
}
