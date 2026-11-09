#include "GalaxyTapeEditor.h"
#include "BinaryData.h"
#include <iterator>

namespace
{
constexpr int editorWidth = 773;
constexpr int editorHeight = 384;
constexpr const char* loopsWebsite = "https://loops.co.ke";

const std::array<GalaxyTapeAudioProcessorEditor::Preset, 13> presets{{
    {"Default",                    360.0f, 0.28f,  0.0f,  0.0f,  0.00f,  0.00f, 0.78f, 0.48f, 0, 1.0f, true,  false, false, false, false},
    {"Vocal - Tight Slap",         120.0f, 0.16f, -1.0f,  1.0f, -0.12f,  0.00f, 0.68f, 0.22f, 0, 1.0f, true,  false, false, false, false},
    {"Vocal - Wide Double",        100.0f, 0.18f, -1.0f,  1.0f, -0.28f,  0.28f, 0.52f, 0.24f, 0, 5.0f, true,  false, true,  false, false},
    {"Vocal - Warm Plate Echo",    420.0f, 0.32f, -1.0f, -1.0f, -0.10f,  0.10f, 0.56f, 0.64f, 1, 7.0f, true,  true,  true,  false, false},
    {"Drums - Tight Room",          74.0f, 0.14f,  1.0f, -1.0f,  0.00f,  0.00f, 0.42f, 0.30f, 0, 1.0f, true,  false, false, false, false},
    {"Drums - Snare Throw",        245.0f, 0.38f,  0.0f,  0.0f, -0.20f,  0.25f, 0.72f, 0.44f, 1, 5.0f, true,  false, true,  false, false},
    {"Bass - Round Tape",           84.0f, 0.17f,  3.0f, -2.0f,  0.00f,  0.00f, 0.55f, 0.18f, 2, 1.0f, true,  false, false, false, false},
    {"Guitar - Rock Slap",         138.0f, 0.24f,  1.0f,  1.0f, -0.10f,  0.10f, 0.68f, 0.25f, 1, 3.0f, true,  true,  false, false, false},
    {"Keys - Stereo Quarter",      500.0f, 0.31f,  0.0f,  1.0f, -0.40f,  0.40f, 0.70f, 0.33f, 1, 5.0f, true,  false, true,  true,  false},
    {"Keys - Dark Echo",           720.0f, 0.43f, -1.0f, -3.0f, -0.25f,  0.25f, 0.65f, 0.42f, 2, 6.0f, false, true,  true,  true,  false},
    {"Dub - Live Feedback",        560.0f, 0.76f,  1.0f, -2.0f, -0.25f,  0.25f, 0.73f, 0.32f, 1, 7.0f, true,  true,  true,  false, false},
    {"Ambient - Long Orbit",       780.0f, 0.69f, -2.0f, -2.0f, -0.35f,  0.35f, 0.64f, 0.78f, 2, 7.0f, true,  true,  true,  false, true},
    {"Studio - Tape Bus",          360.0f, 0.20f, -0.5f, -0.5f,  0.00f,  0.00f, 0.30f, 0.22f, 1, 3.0f, true,  true,  false, false, false}
}};

class GalaxyLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float, float, float, juce::Slider&) override
    {
        juce::ignoreUnused(g, x, y, width, height);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool, bool) override
    {
        juce::ignoreUnused(g, button);
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
    addAndMakeVisible(infoButton);
    addAndMakeVisible(sizeButton);

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
                         &copyButton, &pasteButton, &tapeModeButton, &infoButton, &sizeButton})
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
    infoButton.setTooltip("About Galaxy Tape, Loops, and open-source licenses");
    infoButton.onClick = [this] { showInfoMenu(); };
    sizeButton.setTooltip("Change interface scale");
    sizeButton.onClick = [this] { showSettingsMenu(); };

    addKnob("input_vol", "Input volume", {59, 224, 56, 56});
    addKnob("head_select", "Select echo heads; zero is reverb only", {157, 157, 108, 108}, true);
    addKnob("echo_rate", "Echo time", {285, 145, 56, 56});
    addKnob("treble", "Echo treble", {362, 145, 56, 56});
    addKnob("echo_pan", "Echo pan", {440, 145, 56, 56});
    addKnob("reverb_pan", "Reverb pan", {529, 145, 56, 56});
    addKnob("feedback", "Echo feedback", {285, 224, 56, 56});
    addKnob("bass", "Echo bass", {362, 224, 56, 56});
    addKnob("echo_vol", "Echo level", {440, 224, 56, 56});
    addKnob("reverb_vol", "Reverb level", {529, 224, 56, 56});
    addKnob("output_vol", "Output volume", {644, 224, 56, 56});

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
    slider->setComponentID(id);
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
    knobs.push_back(std::move(slider));
    knobBounds.push_back(bounds);
    addRotaryArtwork(knobs.size() - 1, large ? 39.0f : 22.0f);
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
    switches.push_back(std::move(button));
    switchBounds.push_back(bounds);
}

void GalaxyTapeAudioProcessorEditor::setParameter(const juce::String& id, float value)
{
    if (auto* parameter = processor.parameters.getParameter(id))
        parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
}

void GalaxyTapeAudioProcessorEditor::showSettingsMenu()
{
    juce::PopupMenu menu;
    menu.addSectionHeader("INTERFACE SCALE");

    for (const int percent : {70, 80, 90, 100, 110, 120, 130, 140, 150})
    {
        menu.addItem(juce::String(percent) + "%",
                     [this, percent] { setZoomPercent(percent); });
    }

    menu.addSeparator();
    menu.addItem("Reset to 100%", [this] { setZoomPercent(100); });
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&sizeButton),
                       [](int) {});
}

void GalaxyTapeAudioProcessorEditor::showInfoMenu()
{
    juce::PopupMenu menu;
    menu.addItem("About Galaxy Tape", [this] { showAbout(); });
    menu.addItem("About Loops", [this] { showLoopsAbout(); });
    menu.addItem("Open Loops website",
                 [] { juce::URL(loopsWebsite).launchInDefaultBrowser(); });
    menu.addSeparator();
    menu.addItem("Licenses and notices", [this] { showLicenseInfo(); });
    menu.addItem("Source code and documentation", []
    {
        juce::URL("https://github.com/joe-calih/Galaxy-Tape")
            .launchInDefaultBrowser();
    });
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&infoButton),
                       [](int) {});
}

void GalaxyTapeAudioProcessorEditor::showAbout()
{
    juce::AlertWindow::showMessageBoxAsync(
        juce::MessageBoxIconType::InfoIcon,
        "Galaxy Tape 2.0.0",
        "A studio-ready tape echo and reverb for your VST3 sessions.\n\n"
        "Made for Loops.\n\n"
        "Loops helps music producers discover curated sample packs and sounds "
        "with detailed previews and useful production metadata. Its catalog "
        "covers genres including house, hip-hop, techno, Afrobeats, and "
        "cinematic music. Loops also supports sound creators with distribution, "
        "mastering, and production tools. Sample licensing can vary by pack; "
        "check the terms on Loops before using its samples commercially.\n\n"
        "This plugin is an independent project. Loops is not a manufacturer "
        "of, or affiliated with, SSL audio hardware.",
        "Close", this);
}

void GalaxyTapeAudioProcessorEditor::showLoopsAbout()
{
    juce::AlertWindow::showMessageBoxAsync(
        juce::MessageBoxIconType::InfoIcon,
        "About Loops",
        "Loops is a music-production platform for discovering curated samples "
        "and sound packs. The catalog spans house, hip-hop, techno, Afrobeats, "
        "and cinematic music, with previews and production metadata to help "
        "producers find sounds and creators share their work.\n\n"
        "Visit loops.co.ke for collections, creator information, and the "
        "applicable licensing terms for each sample pack.",
        "Close", this);
}

void GalaxyTapeAudioProcessorEditor::showLicenseInfo()
{
    juce::AlertWindow::showMessageBoxAsync(
        juce::MessageBoxIconType::InfoIcon,
        "Licenses and third-party notices",
        "Galaxy Tape source code: GNU AGPL version 3 or later.\n"
        "JUCE: GNU AGPLv3 for this build.\n"
        "Panel artwork: supplied by the project owner and included with this "
        "AGPL-licensed distribution.\n\n"
        "The complete license and project notices are included in the source "
        "repository. Review the license before redistributing this plugin.",
        "Close", this);
}

void GalaxyTapeAudioProcessorEditor::setZoomPercent(int percent)
{
    zoomPercent = juce::jlimit(70, 150, percent);
    sizeButton.setButtonText(juce::String(zoomPercent) + "%");
    setSize(juce::roundToInt(editorWidth * zoomPercent / 100.0f),
            juce::roundToInt(editorHeight * zoomPercent / 100.0f));
}

void GalaxyTapeAudioProcessorEditor::applyPreset(int index)
{
    if (index < 0 || index >= static_cast<int>(presets.size()))
        return;

    const auto& preset = presets[static_cast<size_t>(index)];
    setParameter("input_vol", 1.0f);
    setParameter("output_vol", 1.0f);
    setParameter("head_select", preset.headSelect);
    setParameter("echo_rate", preset.rate);
    setParameter("treble", preset.treble);
    setParameter("echo_pan", preset.echoPan);
    setParameter("reverb_pan", preset.reverbPan);
    setParameter("feedback", preset.feedback);
    setParameter("bass", preset.bass);
    setParameter("echo_vol", preset.echo);
    setParameter("reverb_vol", preset.reverb);
    setParameter("head_1", preset.head1);
    setParameter("head_2", preset.head2);
    setParameter("head_3", preset.head3);
    setParameter("tempo_sync", preset.tempoSync ? 1.0f : 0.0f);
    setParameter("tape_loop", 0.0f);
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
    displayedOutputPeak = juce::jmax(processor.getOutputPeak(), displayedOutputPeak * 0.88f);
    displayedClip = juce::jmax(processor.getInputPeak() >= 0.98f ? 1.0f : 0.0f,
                                displayedClip * 0.94f);
    const int mode = juce::jlimit(0, 2, juce::roundToInt(
        processor.parameters.getRawParameterValue("tape_mode")->load()));
    static const juce::StringArray names{"NEW", "USED", "OLD"};
    tapeModeButton.setButtonText(names[mode]);
    tapeModeButton.repaint();
    repaint();
}

void GalaxyTapeAudioProcessorEditor::addRotaryArtwork(size_t knobIndex, float radius)
{
    const auto centre = juce::Point<float>(
        static_cast<float>(knobBounds[knobIndex].getCentreX()),
        static_cast<float>(knobBounds[knobIndex].getCentreY()));
    const int extent = juce::roundToInt(radius * 2.0f);
    const int left = juce::roundToInt(centre.x - radius);
    const int top = juce::roundToInt(centre.y - radius);
    juce::Image image(juce::Image::ARGB, extent, extent, true);

    for (int y = 0; y < extent; ++y)
    {
        for (int x = 0; x < extent; ++x)
        {
            const float dx = static_cast<float>(x) - radius + 0.5f;
            const float dy = static_cast<float>(y) - radius + 0.5f;
            const float distance = std::sqrt(dx * dx + dy * dy);
            if (distance > radius)
                continue;

            const float alpha = juce::jlimit(0.0f, 1.0f, radius - distance);
            const auto source = background.getPixelAt(left + x, top + y);
            image.setPixelAt(x, y, source.withAlpha(alpha));
        }
    }

    double defaultPosition = 0.0;
    if (auto* parameter = processor.parameters.getParameter(
            knobs[knobIndex]->getComponentID()))
        defaultPosition = knobs[knobIndex]->valueToProportionOfLength(
            parameter->convertFrom0to1(parameter->getDefaultValue()));

    rotaryArtwork.push_back({std::move(image), centre, radius, knobIndex, defaultPosition});
}

void GalaxyTapeAudioProcessorEditor::drawRotaryArtwork(juce::Graphics& g, float scale)
{
    for (const auto& dial : rotaryArtwork)
    {
        if (dial.image.isNull())
            continue;

        auto& slider = *knobs[dial.knobIndex];
        const float angle = static_cast<float>(
            (slider.valueToProportionOfLength(slider.getValue()) - dial.defaultPosition)
            * juce::MathConstants<double>::twoPi * 0.75);
        const float cosine = std::cos(angle);
        const float sine = std::sin(angle);
        const float left = dial.centre.x - dial.radius;
        const float top = dial.centre.y - dial.radius;
        const float tx = left * cosine - top * sine + dial.centre.x
                         - cosine * dial.centre.x + sine * dial.centre.y;
        const float ty = left * sine + top * cosine + dial.centre.y
                         - sine * dial.centre.x - cosine * dial.centre.y;
        const juce::AffineTransform transform(
            cosine * scale, -sine * scale, tx * scale,
            sine * scale, cosine * scale, ty * scale);
        g.drawImageTransformed(dial.image, transform, false);
    }
}

void GalaxyTapeAudioProcessorEditor::drawBranding(juce::Graphics& g, float scale)
{
    const auto titleBounds = juce::Rectangle<int>(
        juce::roundToInt(259.0f * scale), juce::roundToInt(78.0f * scale),
        juce::roundToInt(254.0f * scale), juce::roundToInt(23.0f * scale));
    const auto subtitleBounds = juce::Rectangle<int>(
        juce::roundToInt(259.0f * scale), juce::roundToInt(99.0f * scale),
        juce::roundToInt(254.0f * scale), juce::roundToInt(12.0f * scale));

    g.setFont(juce::Font(juce::FontOptions("Bahnschrift", 16.0f * scale)
                             .withStyle("SemiBold")));
    g.setColour(juce::Colour(0xff151918).withAlpha(0.7f));
    g.drawText("Galaxy Tape", titleBounds.translated(0, 1),
               juce::Justification::centred, false);
    g.setColour(juce::Colour(0xff343a3b));
    g.drawText("Galaxy Tape", titleBounds, juce::Justification::centred, false);

    g.setFont(juce::Font(juce::FontOptions("Bahnschrift", 7.5f * scale)
                             .withStyle("Medium")));
    g.setColour(juce::Colour(0xff353a3b));
    g.drawText("by loops.co.ke", subtitleBounds, juce::Justification::centred, false);
}

void GalaxyTapeAudioProcessorEditor::drawHeadDisplays(juce::Graphics& g, float scale)
{
    const float rate = juce::jlimit(40.0f, 12000.0f, processor.getEffectiveDelayMs());
    const int mask = juce::roundToInt(
        processor.parameters.getRawParameterValue("head_select")->load());
    const float ratios[3] = {1.0f, 0.6666667f, 0.5f};

    for (int head = 0; head < 3; ++head)
    {
        const auto bounds = juce::Rectangle<float>(
            (155.0f + head * 56.0f) * scale, 315.0f * scale,
            46.0f * scale, 17.0f * scale);
        const bool active = switches[static_cast<size_t>(head)]->getToggleState()
                            && (mask & (1 << head)) != 0;
        const auto top = active ? juce::Colour(0xff102616)
                                : juce::Colour(0xff0a140d);
        const auto bottom = active ? juce::Colour(0xff020805)
                                   : juce::Colour(0xff030705);
        g.setGradientFill(juce::ColourGradient(top, bounds.getCentreX(), bounds.getY(),
                                                bottom, bounds.getCentreX(),
                                                bounds.getBottom(), false));
        g.fillRoundedRectangle(bounds, 1.4f * scale);
        g.setColour(juce::Colour(0xff88a183).withAlpha(active ? 0.72f : 0.36f));
        g.drawRoundedRectangle(bounds, 1.4f * scale, 0.7f * scale);

        const int milliseconds = juce::jlimit(0, 9999,
            juce::roundToInt(rate * ratios[head]));
        const auto digits = juce::String(milliseconds).paddedLeft('0', 4);
        const auto digitBounds = juce::Rectangle<int>(
            juce::roundToInt(bounds.getX() + 1.0f * scale),
            juce::roundToInt(bounds.getY()),
            juce::roundToInt(bounds.getWidth() - 7.0f * scale),
            juce::roundToInt(bounds.getHeight()));

        g.setFont(juce::Font(juce::FontOptions("Consolas", 9.0f * scale)
                                 .withStyle("Bold")));
        g.setColour(active ? juce::Colour(0xff42ff82).withAlpha(0.22f)
                           : juce::Colour(0xff39c56a).withAlpha(0.10f));
        g.drawFittedText(digits, digitBounds.expanded(1, 0),
                         juce::Justification::centred, 1, 0.8f * scale);
        g.setColour(active ? juce::Colour(0xff92ffad)
                           : juce::Colour(0xff487358));
        g.drawFittedText(digits, digitBounds,
                         juce::Justification::centred, 1, 0.8f * scale);

        g.setFont(juce::Font(juce::FontOptions("Bahnschrift", 5.2f * scale)
                                 .withStyle("Medium")));
        g.setColour(active ? juce::Colour(0xff87d498)
                           : juce::Colour(0xff48604d));
        g.drawText("ms", juce::roundToInt(bounds.getRight() - 8.0f * scale),
                   juce::roundToInt(bounds.getY()), juce::roundToInt(7.0f * scale),
                   juce::roundToInt(bounds.getHeight()),
                   juce::Justification::centred, false);
    }
}

void GalaxyTapeAudioProcessorEditor::drawSwitches(juce::Graphics& g, float scale)
{
    for (size_t index = 3; index < switches.size(); ++index)
    {
        const auto original = switchBounds[index].toFloat();
        const auto bounds = juce::Rectangle<float>(
            original.getX() * scale, original.getY() * scale,
            original.getWidth() * scale, original.getHeight() * scale);
        const float cx = bounds.getCentreX();
        const float cy = bounds.getCentreY();
        const bool enabled = switches[index]->getToggleState();
        const float direction = enabled ? 1.0f : -1.0f;
        const float endX = cx + direction * 7.0f * scale;
        const float endY = cy - 3.5f * scale;
        const float thickness = juce::jmax(1.0f, 3.6f * scale);

        g.setColour(juce::Colour(0xb9000000));
        g.fillEllipse(cx - 8.0f * scale, cy - 8.0f * scale,
                      16.0f * scale, 16.0f * scale);
        g.setColour(juce::Colour(0xffb6b9b1));
        g.drawEllipse(cx - 7.0f * scale, cy - 7.0f * scale,
                      14.0f * scale, 14.0f * scale, 0.8f * scale);
        g.setColour(juce::Colour(0xff171a18));
        g.drawLine(cx, cy, endX, endY, thickness + 1.5f * scale);
        g.setColour(juce::Colour(0xffe6e8df));
        g.drawLine(cx - 0.5f * scale, cy - 0.3f * scale,
                   endX - 0.5f * scale, endY - 0.3f * scale, thickness);

        juce::ColourGradient cap(juce::Colour(0xfff0eee3),
                                 endX - 2.0f * scale, endY - 2.0f * scale,
                                 juce::Colour(0xff73766d),
                                 endX + 2.0f * scale, endY + 2.0f * scale, false);
        g.setGradientFill(cap);
        g.fillEllipse(endX - 3.0f * scale, endY - 3.0f * scale,
                      6.0f * scale, 6.0f * scale);
        g.setColour(juce::Colour(0xff555950));
        g.drawEllipse(endX - 3.0f * scale, endY - 3.0f * scale,
                      6.0f * scale, 6.0f * scale, 0.7f * scale);
    }
}

void GalaxyTapeAudioProcessorEditor::drawPeakIndicator(juce::Graphics& g, float scale)
{
    const float intensity = juce::jlimit(0.0f, 1.0f, displayedClip);
    const float alpha = 0.14f + intensity * 0.86f;
    const auto centre = juce::Point<float>(87.0f * scale, 155.0f * scale);
    const float radius = 6.5f * scale;

    juce::ColourGradient lamp(
        juce::Colour(0xffffb6a1).withAlpha(alpha),
        centre.x - radius * 0.4f, centre.y - radius * 0.5f,
        juce::Colour(0xffb00808).withAlpha(alpha),
        centre.x + radius * 0.6f, centre.y + radius * 0.8f, true);
    g.setColour(juce::Colour(0x44ff3c18).withAlpha(0.15f + intensity * 0.55f));
    g.fillEllipse(centre.x - radius - 2.0f * scale,
                  centre.y - radius - 2.0f * scale,
                  (radius + 2.0f * scale) * 2.0f,
                  (radius + 2.0f * scale) * 2.0f);
    g.setGradientFill(lamp);
    g.fillEllipse(centre.x - radius, centre.y - radius,
                  radius * 2.0f, radius * 2.0f);
    g.setColour(juce::Colour(0xffffdddd).withAlpha(0.35f + intensity * 0.65f));
    g.fillEllipse(centre.x - 2.0f * scale, centre.y - 3.5f * scale,
                  3.0f * scale, 2.2f * scale);
}

void GalaxyTapeAudioProcessorEditor::drawMeter(juce::Graphics& g)
{
    const float scale = zoomPercent / 100.0f;
    const auto face = juce::Rectangle<float>(628.0f * scale, 146.0f * scale,
                                              87.0f * scale, 38.0f * scale);
    juce::ColourGradient glass(juce::Colour(0xff151b1a),
                               face.getCentreX(), face.getY(),
                               juce::Colour(0xff080d0e),
                               face.getCentreX(), face.getBottom(), false);
    glass.addColour(0.42, juce::Colour(0xff111518));
    g.setGradientFill(glass);
    g.fillRoundedRectangle(face, 2.0f * scale);
    g.setColour(juce::Colour(0xff778078).withAlpha(0.8f));
    g.drawRoundedRectangle(face, 2.0f * scale, 0.8f * scale);

    static const char* labels[]{"15", "10", "7", "5", "3", "0"};
    static const float labelPositions[]{636.0f, 650.0f, 665.0f, 678.0f, 691.0f, 704.0f};
    g.setFont(juce::Font(juce::FontOptions("Bahnschrift", 5.5f * scale)
                             .withStyle("SemiBold")));
    g.setColour(juce::Colour(0xff96c2a4));
    for (size_t i = 0; i < std::size(labels); ++i)
        g.drawText(labels[i], juce::roundToInt(labelPositions[i] * scale),
                   juce::roundToInt(148.0f * scale), juce::roundToInt(12.0f * scale),
                   juce::roundToInt(7.0f * scale),
                   juce::Justification::centred, false);

    for (int segment = 0; segment < 12; ++segment)
    {
        const float x = (635.0f + segment * 5.9f) * scale;
        const bool amber = segment >= 9;
        const auto colour = amber ? juce::Colour(0xffff922f)
                                  : juce::Colour(0xff4cf38c);
        const auto bounds = juce::Rectangle<float>(
            x, 158.0f * scale, 4.7f * scale, 6.5f * scale);
        g.setColour(colour.withAlpha(0.16f));
        g.fillRoundedRectangle(bounds.expanded(1.2f * scale), 1.0f * scale);
        juce::ColourGradient led(colour.brighter(0.35f),
                                 bounds.getCentreX(), bounds.getY(),
                                 colour.darker(0.3f),
                                 bounds.getCentreX(), bounds.getBottom(), false);
        g.setGradientFill(led);
        g.fillRoundedRectangle(bounds, 0.8f * scale);
        g.setColour(juce::Colours::white.withAlpha(0.38f));
        g.drawHorizontalLine(juce::roundToInt(bounds.getY()),
                             bounds.getX() + 0.6f * scale,
                             bounds.getRight() - 0.6f * scale);
    }

    g.setColour(juce::Colour(0xff0b1011));
    g.drawLine(677.0f * scale, 193.0f * scale,
               680.0f * scale, 183.0f * scale, 2.8f * scale);

    const double levelDb = juce::Decibels::gainToDecibels(
        juce::jmax(displayedOutputPeak, 0.000001f), -60.0f);
    const float normalised = juce::jlimit(
        0.0f, 1.0f, static_cast<float>((levelDb + 24.0) / 27.0));
    const float angle = juce::degreesToRadians(225.0f + 78.0f * normalised);
    const juce::Point<float> pivot(682.0f * scale, 190.0f * scale);
    const juce::Point<float> tip(
        pivot.x + std::cos(angle) * 42.0f * scale,
        pivot.y + std::sin(angle) * 42.0f * scale);

    g.setColour(juce::Colour(0x9a000000));
    g.drawLine(pivot.x + 0.9f * scale, pivot.y + 0.8f * scale,
               tip.x + 0.9f * scale, tip.y + 0.8f * scale, 2.8f * scale);
    g.setColour(juce::Colour(0xffffeed5));
    g.drawLine(pivot.x, pivot.y, tip.x, tip.y, 1.45f * scale);
    g.setColour(juce::Colour(0xffffa249));
    g.drawLine(pivot.x, pivot.y, tip.x, tip.y, 0.65f * scale);
    juce::ColourGradient hub(juce::Colour(0xffffdeb0),
                             pivot.x - 1.5f * scale, pivot.y - 1.5f * scale,
                             juce::Colour(0xff925322),
                             pivot.x + 1.5f * scale, pivot.y + 1.5f * scale, false);
    g.setGradientFill(hub);
    g.fillEllipse(pivot.x - 2.5f * scale, pivot.y - 2.5f * scale,
                  5.0f * scale, 5.0f * scale);
}

void GalaxyTapeAudioProcessorEditor::paint(juce::Graphics& g)
{
    const float scale = zoomPercent / 100.0f;
    if (background.isValid())
    {
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImage(background, getLocalBounds().toFloat());
    }
    else
        g.fillAll(juce::Colour(0xff171a18));
    drawRotaryArtwork(g, scale);
    drawBranding(g, scale);
    drawHeadDisplays(g, scale);
    drawSwitches(g, scale);
    drawPeakIndicator(g, scale);
    drawMeter(g);
}

void GalaxyTapeAudioProcessorEditor::resized()
{
    const float scale = zoomPercent / 100.0f;
    const auto scaled = [scale](juce::Rectangle<int> bounds)
    {
        return juce::Rectangle<int>(juce::roundToInt(bounds.getX() * scale),
                                    juce::roundToInt(bounds.getY() * scale),
                                    juce::roundToInt(bounds.getWidth() * scale),
                                    juce::roundToInt(bounds.getHeight() * scale));
    };

    const juce::Rectangle<int> headerBounds[]{
        {5, 5, 24, 25}, {29, 5, 20, 25}, {49, 5, 220, 25},
        {298, 6, 24, 24}, {323, 6, 24, 24}, {350, 6, 38, 24},
        {390, 6, 39, 24}, {453, 315, 61, 25}, {676, 5, 50, 25},
        {729, 5, 39, 25}
    };
    juce::Component* headerComponents[]{
        &previousPreset, &nextPreset, &presetBox, &aButton, &bButton,
        &copyButton, &pasteButton, &tapeModeButton, &infoButton, &sizeButton
    };
    for (size_t i = 0; i < std::size(headerComponents); ++i)
        headerComponents[i]->setBounds(scaled(headerBounds[i]));

    for (size_t i = 0; i < knobs.size(); ++i)
        knobs[i]->setBounds(scaled(knobBounds[i]));
    for (size_t i = 0; i < switches.size(); ++i)
        switches[i]->setBounds(scaled(switchBounds[i]));
}
