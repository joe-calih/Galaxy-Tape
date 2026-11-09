#pragma once

#include "GalaxyTapeProcessor.h"

class GalaxyTapeAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                             private juce::Timer
{
public:
    struct Preset
    {
        const char* name;
        float rate;
        float feedback;
        float bass;
        float treble;
        float echoPan;
        float reverbPan;
        float echo;
        float reverb;
        float tapeMode;
        float headSelect;
        bool head1;
        bool head2;
        bool head3;
        bool tempoSync;
        float wetSolo;
    };

    struct RotaryArtwork
    {
        juce::Image image;
        juce::Point<float> centre;
        float radius = 0.0f;
        size_t knobIndex = 0;
        double defaultPosition = 0.0;
    };

    explicit GalaxyTapeAudioProcessorEditor(GalaxyTapeAudioProcessor&);
    ~GalaxyTapeAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void addKnob(const juce::String& id, const juce::String& name,
                 juce::Rectangle<int> bounds, bool large = false);
    void addSwitch(const juce::String& id, const juce::String& name,
                   juce::Rectangle<int> bounds);
    void applyPreset(int index);
    void selectComparison(int slot);
    void setParameter(const juce::String& id, float value);
    void showSettingsMenu();
    void showInfoMenu();
    void showAbout();
    void showLoopsAbout();
    void showLicenseInfo();
    void setZoomPercent(int percent);
    void addRotaryArtwork(size_t knobIndex, float radius);
    void drawRotaryArtwork(juce::Graphics& graphics, float scale);
    void drawBranding(juce::Graphics& graphics, float scale);
    void drawHeadDisplays(juce::Graphics& graphics, float scale);
    void drawSwitches(juce::Graphics& graphics, float scale);
    void drawPeakIndicator(juce::Graphics& graphics, float scale);
    void drawMeter(juce::Graphics& graphics);

    GalaxyTapeAudioProcessor& processor;
    juce::Image background;
    juce::ComboBox presetBox;
    juce::TextButton previousPreset{"<"};
    juce::TextButton nextPreset{">"};
    juce::TextButton aButton{"A"};
    juce::TextButton bButton{"B"};
    juce::TextButton copyButton{"COPY"};
    juce::TextButton pasteButton{"PASTE"};
    juce::TextButton tapeModeButton;
    juce::TextButton infoButton{"INFO"};
    juce::TextButton sizeButton{"100%"};
    std::vector<std::unique_ptr<juce::Slider>> knobs;
    std::vector<juce::Rectangle<int>> knobBounds;
    std::vector<RotaryArtwork> rotaryArtwork;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<juce::ToggleButton>> switches;
    std::vector<juce::Rectangle<int>> switchBounds;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;
    std::array<juce::ValueTree, 2> comparisonStates;
    juce::ValueTree copiedState;
    int activeComparison = 0;
    float displayedOutputPeak = 0.0f;
    float displayedClip = 0.0f;
    int zoomPercent = 100;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GalaxyTapeAudioProcessorEditor)
};
