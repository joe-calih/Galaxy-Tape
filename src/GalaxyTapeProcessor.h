#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <vector>

class GalaxyTapeAudioProcessor final : public juce::AudioProcessor
{
public:
    GalaxyTapeAudioProcessor();
    ~GalaxyTapeAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 8.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destination) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;
    float getInputPeak() const noexcept
    {
        return inputPeak.load(std::memory_order_relaxed);
    }
    float getOutputPeak() const noexcept
    {
        return outputPeak.load(std::memory_order_relaxed);
    }
    float getEffectiveDelayMs() const noexcept
    {
        return effectiveDelayMs.load(std::memory_order_relaxed);
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    float readDelaySample(int channel, float delaySamples) const noexcept;
    static float readParameter(const juce::AudioProcessorValueTreeState&,
                               const juce::String& id) noexcept;

    std::array<std::vector<float>, 2> delayBuffer;
    juce::AudioBuffer<float> reverbBuffer;
    juce::Reverb reverb;
    std::array<float, 2> lowBandState{};
    std::array<float, 2> highBandState{};
    int writePosition = 0;
    int currentReverbMode = -1;
    double currentSampleRate = 44100.0;
    double tapePhase = 0.0;
    std::atomic<float> inputPeak{0.0f};
    std::atomic<float> outputPeak{0.0f};
    std::atomic<float> effectiveDelayMs{360.0f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GalaxyTapeAudioProcessor)
};
