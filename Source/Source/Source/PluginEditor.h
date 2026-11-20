#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class HuntericeAbyssAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    HuntericeAbyssAudioProcessorEditor(HuntericeAbyssAudioProcessor&,
                                       juce::AudioProcessorValueTreeState&);
    ~HuntericeAbyssAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    HuntericeAbyssAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::Slider masterVolSlider;
    juce::Slider oscLevelSlider;
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> masterVolAttachment;
    std::unique_ptr<SliderAttachment> oscLevelAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuntericeAbyssAudioProcessorEditor)
};
