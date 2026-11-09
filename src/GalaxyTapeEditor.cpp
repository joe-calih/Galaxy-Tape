#include "GalaxyTapeEditor.h"
#include "BinaryData.h"

namespace
{
constexpr int editorWidth = 773;
constexpr int editorHeight = 384;

const std::array<GalaxyTapeAudioProcessorEditor::Preset, 5> presets{{
    {"Default",       1.00f, 4.0f, 360.0f,  0.0f,  0.0f,  0.0f, 0.28f,  0.0f, 0.78f, 0.48f, 1.00f, 1, 1, 1, 0, 0, 0, 0},
    {"Slapback",      1.00f, 1.0f, 125.0f,  1.0f,  0.0f,  0.0f, 0.16f,  0.0f, 0.72f, 0.28f, 1.00f, 1, 0, 0, 0, 0, 0, 0},
    {"Galactic Wash", 0.90f, 7.0f, 690.0f, -2.0f, -0.3f,  0.3f, 0.62f, -1.0f, 0.65f, 0.82f, 0.92f, 1, 1, 1, 0, 0, 1, 0},
    {"Old Orbit",     1.00f, 5.0f, 485.0f, -4.0f,  0.2f, -0.2f, 0.51f,  1.0f, 0.82f, 0.54f, 0.95f, 1, 0, 1, 0, 0, 2, 0},
    {"Dub Loop",      0.85f, 7.0f, 740.0f, -1.0f,  0.0f,  0.0f, 0.79f,  1.0f, 0.76f, 0.38f, 0.88f, 1, 1, 1, 1, 0, 1, 0}
}};

class GalaxyLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool, bool) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        const bool active = button.getToggleState();

        if (bounds.getWidth() > 40.0f)
        {
            g.setColour(juce::Colour(0xff080d0a));
            g.fillRoundedRectangle(bounds, 3.0f);
            g.setColour(juce::Colour(0xff778078));
            g.drawRoundedRectangle(bounds, 3.0f, 1.0f);
            for (int led = 0; led < 5; ++led)
            {
                const float x = bounds.getX() + 7.0f + led * 7.2f;
                g.setColour(active ? juce::Colour(0xff65e773)
                                   : juce::Colour(0xff174626));
                g.fillRoundedRectangle(x, bounds.getCentreY() - 3.0f, 5.0f, 6.0f, 1.0f);
            }
            g.setColour(active ? juce::Colour(0xff71f281)
                               : juce::Colour(0xff14522d));
            g.fillRoundedRectangle(bounds.getRight() - 9.0f,
                                   bounds.getCentreY() - 3.0f, 5.0f, 6.0f, 1.0f);
            return;
        }

        g.setColour(juce::Colour(0xff101310));
        g.fillEllipse(bounds.reduced(bounds.getWidth() * 0.12f));
        g.setColour(juce::Colour(0xffb1b5ad));
        g.drawEllipse(bounds.reduced(bounds.getWidth() * 0.12f), 1.0f);
        const float cx = bounds.getCentreX();
        const float cy = bounds.getCentreY();
        const float leverY = active ? bounds.getY() + 5.0f : bounds.getBottom() - 5.0f;
        g.setColour(juce::Colour(0xff161916));
        g.drawLine(cx, cy, cx, leverY, 4.0f);
        g.setColour(juce::Colour(0xffd5d8d1));
        g.fillEllipse(cx - 4.0f, leverY - 4.0f, 8.0f, 8.0f);
        g.setColour(juce::Colour(0xff626760));
        g.drawEllipse(cx - 4.0f, leverY - 4.0f, 8.0f, 8.0f, 1.0f);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosition, float startAngle, float endAngle,
                          juce::Slider&) override
    {
        const float diameter = static_cast<float>(juce::jmin(width, height));
        const float cx = x + width * 0.5f;
        const float cy = y + height * 0.5f;
        const float outer = diameter * 0.48f;
        const float radius = diameter * 0.34f;

        g.setColour(juce::Colour(0xff080a09));
        g.fillEllipse(cx - outer, cy - outer, outer * 2.0f, outer * 2.0f);
        g.setColour(juce::Colour(0xff777b75));
        g.drawEllipse(cx - outer, cy - outer, outer * 2.0f, outer * 2.0f, 1.1f);

        for (int tick = 0; tick <= 10; ++tick)
        {
            const float proportion = static_cast<float>(tick) / 10.0f;
            const float angle = startAngle + proportion * (endAngle - startAngle);
            const float innerRadius = outer * (tick % 5 == 0 ? 0.76f : 0.83f);
            const float outerRadius = outer * 0.94f;
            g.setColour(tick / 10.0f <= sliderPosition
                            ? juce::Colour(0xff9ecb83)
                            : juce::Colour(0xff9b9e97));
            g.drawLine(cx + std::cos(angle) * innerRadius,
                       cy + std::sin(angle) * innerRadius,
                       cx + std::cos(angle) * outerRadius,
                       cy + std::sin(angle) * outerRadius,
                       tick % 5 == 0 ? 1.6f : 1.0f);
        }

        juce::ColourGradient face(juce::Colour(0xffe6e7e2), cx - radius * 0.5f,
                                  cy - radius * 0.75f, juce::Colour(0xff404440),
                                  cx + radius * 0.55f, cy + radius * 0.9f, false);
        face.addColour(0.48, juce::Colour(0xffaeb1aa));
        g.setGradientFill(face);
        g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
        g.setColour(juce::Colour(0xff171a17));
        g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 2.0f);

        const float angle = startAngle + sliderPosition * (endAngle - startAngle);
        g.setColour(juce::Colour(0xff121512));
        g.drawLine(cx, cy, cx + std::cos(angle) * radius * 0.76f,
                   cy + std::sin(angle) * radius * 0.76f, 3.0f);
        g.setColour(juce::Colour(0xfff4f4ef));
        g.fillEllipse(cx - 2.0f, cy - 2.0f, 4.0f, 4.0f);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour&, bool, bool) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        const bool active = button.getToggleState();
        g.setColour(juce::Colour(0xffa9b0a7));
        g.fillRoundedRectangle(bounds.translated(0.0f, 1.0f), 3.0f);
        juce::ColourGradient gradient(active ? juce::Colour(0xff435a37)
                                             : juce::Colour(0xff343936),
                                      bounds.getCentreX(), bounds.getY(),
                                      active ? juce::Colour(0xff92ae6d)
                                             : juce::Colour(0xff111412),
                                      bounds.getCentreX(), bounds.getBottom(), false);
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds.reduced(1.0f), 3.0f);
        g.setColour(active ? juce::Colour(0xffd5eab8) : juce::Colour(0xff828781));
        g.drawRoundedRectangle(bounds.reduced(1.0f), 3.0f, 1.0f);
    }
};

GalaxyLookAndFeel galaxyLookAndFeel;
}

GalaxyTapeAudioProcessorEditor::GalaxyTapeAudioProcessorEditor(
    GalaxyTapeAudioProcessor& audioProcessor)
    : AudioProcessorEditor(&audioProcessor), processor(audioProcessor)
{
    background = juce::ImageFileFormat::loadFrom(
        BinaryData::GalaxyTape_png, BinaryData::GalaxyTape_pngSize);

    setSize(editorWidth, editorHeight);
    setResizable(false, false);

    addAndMakeVisible(previousPreset);
    addAndMakeVisible(nextPreset);
    addAndMakeVisible(presetBox);
    addAndMakeVisible(aButton);
    addAndMakeVisible(bButton);
    addAndMakeVisible(copyButton);
    addAndMakeVisible(pasteButton);
    addAndMakeVisible(tapeModeButton);

    for (const auto& preset : presets)
        presetBox.addItem(preset.name, presetBox.getNumItems() + 1);
    presetBox.setSelectedId(1, juce::dontSendNotification);
    presetBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff383c39));
    presetBox.setColour(juce::ComboBox::textColourId, juce::Colour(0xffe2e4df));
    presetBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff686d67));
    presetBox.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xffb8bdb6));
    presetBox.setTooltip("Factory preset");
    presetBox.onChange = [this] { applyPreset(presetBox.getSelectedItemIndex()); };

    auto styleHeaderButton = [](juce::TextButton& button)
    {
        button.setLookAndFeel(&galaxyLookAndFeel);
        button.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffe1e4dc));
        button.setColour(juce::TextButton::textColourOnId, juce::Colour(0xffe1e4dc));
    };
    for (auto* button : {&previousPreset, &nextPreset, &aButton, &bButton,
                         &copyButton, &pasteButton, &tapeModeButton})
        styleHeaderButton(*button);

    previousPreset.onClick = [this]
    {
        const int next = (presetBox.getSelectedItemIndex() + static_cast<int>(presets.size()) - 1)
                         % static_cast<int>(presets.size());
        presetBox.setSelectedItemIndex(next);
    };
    nextPreset.onClick = [this]
    {
        const int next = (presetBox.getSelectedItemIndex() + 1)
                         % static_cast<int>(presets.size());
        presetBox.setSelectedItemIndex(next);
    };
    aButton.onClick = [this] { selectComparison(0); };
    bButton.onClick = [this] { selectComparison(1); };
    aButton.setClickingTogglesState(false);
    bButton.setClickingTogglesState(false);
    copyButton.onClick = [this] { copiedState = processor.parameters.copyState(); };
    pasteButton.onClick = [this]
    {
        if (copiedState.isValid())
            processor.parameters.replaceState(copiedState.createCopy());
    };

    addKnob("input_vol", "Input volume", {57, 220, 61, 63});
    addKnob("head_select", "Select echo heads; zero is reverb only", {158, 156, 106, 106}, true);
    addKnob("echo_rate", "Echo time", {284, 147, 59, 59});
    addKnob("treble", "Echo treble", {361, 147, 59, 59});
    addKnob("echo_pan", "Echo pan", {439, 147, 59, 59});
    addKnob("reverb_pan", "Reverb pan", {527, 147, 59, 59});
    addKnob("feedback", "Echo feedback", {284, 226, 59, 59});
    addKnob("bass", "Echo bass", {361, 226, 59, 59});
    addKnob("echo_vol", "Echo level", {439, 226, 59, 59});
    addKnob("reverb_vol", "Reverb level", {527, 226, 59, 59});
    addKnob("output_vol", "Output volume", {642, 220, 61, 63});

    addSwitch("head_1", "Head 1", {151, 311, 54, 27});
    addSwitch("head_2", "Head 2", {208, 311, 54, 27});
    addSwitch("head_3", "Head 3", {265, 311, 54, 27});
    addSwitch("input_send", "Input send / echo mute", {72, 312, 31, 31});
    addSwitch("tempo_sync", "Tempo sync", {344, 312, 31, 31});
    addSwitch("wet_solo", "Wet solo", {542, 312, 31, 31});
    addSwitch("power", "Power", {657, 312, 31, 31});
    addSwitch("tape_loop", "Tape loop hold", {412, 312, 30, 31});
    tapeModeButton.setTooltip("Click to select New, Used, or Old tape stock");
    tapeModeButton.onClick = [this]
    {
        const int next = (juce::roundToInt(
            processor.parameters.getRawParameterValue("tape_mode")->load()) + 1) % 3;
        setParameter("tape_mode", static_cast<float>(next));
        repaint();
    };

    comparisonStates[0] = processor.parameters.copyState();
    comparisonStates[1] = processor.parameters.copyState();
    selectComparison(0);
    startTimerHz(30);
}

void GalaxyTapeAudioProcessorEditor::addKnob(const juce::String& id,
                                              const juce::String& name,
                                              juce::Rectangle<int> bounds, bool large)
{
    auto slider = std::make_unique<juce::Slider>();
    slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider->setLookAndFeel(&galaxyLookAndFeel);
    slider->setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                                juce::MathConstants<float>::pi * 2.75f, true);
    if (auto* parameter = processor.parameters.getParameter(id))
        slider->setDoubleClickReturnValue(true,
            parameter->convertFrom0to1(parameter->getDefaultValue()));
    slider->setTooltip(name);
    addAndMakeVisible(*slider);
    sliderAttachments.push_back(std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, id, *slider));
    if (large)
        slider->setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff9dc785));
    slider->setBounds(bounds);
    knobs.push_back(std::move(slider));
}

void GalaxyTapeAudioProcessorEditor::addSwitch(const juce::String& id,
                                                const juce::String& name,
                                                juce::Rectangle<int> bounds)
{
    auto button = std::make_unique<juce::ToggleButton>();
    button->setButtonText({});
    button->setClickingTogglesState(true);
    button->setLookAndFeel(&galaxyLookAndFeel);
    button->setTooltip(name);
    addAndMakeVisible(*button);
    buttonAttachments.push_back(std::make_unique<
        juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, id, *button));
    button->setBounds(bounds);
    switches.push_back(std::move(button));
}

void GalaxyTapeAudioProcessorEditor::setParameter(const juce::String& id, float value)
{
    if (auto* parameter = processor.parameters.getParameter(id))
        parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
}

void GalaxyTapeAudioProcessorEditor::applyPreset(int index)
{
    if (index < 0 || index >= static_cast<int>(presets.size()))
        return;

    const auto& preset = presets[static_cast<size_t>(index)];
    setParameter("input_vol", preset.input);
    setParameter("head_select", preset.head);
    setParameter("echo_rate", preset.rate);
    setParameter("treble", preset.treble);
    setParameter("echo_pan", preset.echoPan);
    setParameter("reverb_pan", preset.reverbPan);
    setParameter("feedback", preset.feedback);
    setParameter("bass", preset.bass);
    setParameter("echo_vol", preset.echo);
    setParameter("reverb_vol", preset.reverb);
    setParameter("output_vol", preset.output);
    setParameter("head_1", preset.head1);
    setParameter("head_2", preset.head2);
    setParameter("head_3", preset.head3);
    setParameter("tempo_sync", preset.sync);
    setParameter("tape_loop", preset.loop);
    setParameter("tape_mode", preset.tapeMode);
    setParameter("wet_solo", preset.wetSolo);
    setParameter("input_send", 1.0f);
    setParameter("power", 1.0f);
    comparisonStates[static_cast<size_t>(activeComparison)] = processor.parameters.copyState();
}

void GalaxyTapeAudioProcessorEditor::selectComparison(int slot)
{
    if (slot != activeComparison)
    {
        comparisonStates[static_cast<size_t>(activeComparison)] = processor.parameters.copyState();
        if (comparisonStates[static_cast<size_t>(slot)].isValid())
            processor.parameters.replaceState(comparisonStates[static_cast<size_t>(slot)].createCopy());
        activeComparison = slot;
    }
    aButton.setColour(juce::TextButton::buttonOnColourId,
                      slot == 0 ? juce::Colour(0xff4b623d) : juce::Colour(0xff242725));
    bButton.setColour(juce::TextButton::buttonOnColourId,
                      slot == 1 ? juce::Colour(0xff4b623d) : juce::Colour(0xff242725));
    aButton.setToggleState(slot == 0, juce::dontSendNotification);
    bButton.setToggleState(slot == 1, juce::dontSendNotification);
}

void GalaxyTapeAudioProcessorEditor::timerCallback()
{
    displayedInputPeak = juce::jmax(processor.getInputPeak(), displayedInputPeak * 0.88f);
    displayedOutputPeak = juce::jmax(processor.getOutputPeak(), displayedOutputPeak * 0.88f);
    const int mode = juce::jlimit(0, 2, juce::roundToInt(
        processor.parameters.getRawParameterValue("tape_mode")->load()));
    static const juce::StringArray names{"NEW", "USED", "OLD"};
    tapeModeButton.setButtonText(names[mode]);
    tapeModeButton.repaint();
    repaint(620, 137, 103, 55);
}

void GalaxyTapeAudioProcessorEditor::drawMeter(juce::Graphics& g)
{
    const auto frame = juce::Rectangle<float>(620.0f, 137.0f, 103.0f, 55.0f);
    g.setColour(juce::Colour(0xff111412));
    g.fillRoundedRectangle(frame, 3.0f);
    g.setColour(juce::Colour(0xff777d77));
    g.drawRoundedRectangle(frame, 3.0f, 1.2f);

    const auto screen = juce::Rectangle<float>(630.0f, 148.0f, 82.0f, 33.0f);
    g.setColour(juce::Colour(0xff07110b));
    g.fillRoundedRectangle(screen, 1.0f);
    const float level = juce::jlimit(0.0f, 1.0f, displayedOutputPeak);
    const float width = screen.getWidth() * level;
    g.setColour(juce::Colour(0xff16844d));
    g.fillRect(screen.getX(), screen.getY() + 11.0f, juce::jmin(width, 65.0f), 9.0f);
    if (width > 65.0f)
    {
        g.setColour(juce::Colour(0xffd99844));
        g.fillRect(screen.getX() + 65.0f, screen.getY() + 11.0f,
                   juce::jmin(width - 65.0f, 17.0f), 9.0f);
    }
    const float needleX = screen.getX() + juce::jlimit(0.0f, 1.0f, displayedInputPeak)
                                                 * (screen.getWidth() - 5.0f);
    g.setColour(juce::Colour(0xffffb969));
    g.drawLine(needleX, screen.getY() + 3.0f, needleX - 7.0f,
               screen.getBottom() - 3.0f, 2.0f);
    g.setColour(juce::Colour(0xffcae5a8));
    g.drawHorizontalLine(static_cast<int>(screen.getY() + 5.0f),
                         screen.getX() + 6.0f, screen.getRight() - 6.0f);
}

void GalaxyTapeAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (background.isValid())
        g.drawImage(background, getLocalBounds().toFloat());
    else
        g.fillAll(juce::Colour(0xff171a18));
    drawMeter(g);
}

void GalaxyTapeAudioProcessorEditor::resized()
{
    previousPreset.setBounds(5, 5, 24, 25);
    nextPreset.setBounds(29, 5, 20, 25);
    presetBox.setBounds(49, 5, 220, 25);
    aButton.setBounds(298, 6, 24, 24);
    bButton.setBounds(323, 6, 24, 24);
    copyButton.setBounds(350, 6, 38, 24);
    pasteButton.setBounds(390, 6, 39, 24);
    tapeModeButton.setBounds(453, 315, 61, 25);
}
