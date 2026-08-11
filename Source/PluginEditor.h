#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ViolentGlitchEditor : public juce::AudioProcessorEditor
{
public:
    ViolentGlitchEditor(ViolentGlitchProcessor&);
    ~ViolentGlitchEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ViolentGlitchProcessor& audioProcessor;
    
    juce::Slider crushSlider;
    juce::Slider rateSlider;
    juce::Slider chaosSlider;
    juce::Slider mixSlider;
    
    juce::Label crushLabel;
    juce::Label rateLabel;
    juce::Label chaosLabel;
    juce::Label mixLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crushAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chaosAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ViolentGlitchEditor)
};
