#pragma once

#include "GalaxyTapeProcessor.h"

class GalaxyTapeAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                             private juce::Timer
{
public:
    struct Preset
    {
        const char* name;
        float input;
        float head;
        float rate;
        float treble;
        float echoPan;
        float reverbPan;
        float feedback;
        float bass;
        float echo;
        float reverb;
        float output;
        float head1;
        float head2;
        float head3;
        float sync;
        float loop;
        float tapeMode;
        float wetSolo;
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
    std::vector<std::unique_ptr<juce::Slider>> knobs;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<juce::ToggleButton>> switches;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;
    std::array<juce::ValueTree, 2> comparisonStates;
    juce::ValueTree copiedState;
    int activeComparison = 0;
    float displayedInputPeak = 0.0f;
    float displayedOutputPeak = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GalaxyTapeAudioProcessorEditor)
};
