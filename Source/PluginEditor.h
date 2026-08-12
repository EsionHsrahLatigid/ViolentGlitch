#pragma once
#include <JuceHeader.h>
#include <ehl/juce_design/EhlDesign.h>
#include "PluginProcessor.h"

class ViolentGlitchEditor : public juce::AudioProcessorEditor,
                            private juce::Timer
{
public:
    ViolentGlitchEditor(ViolentGlitchProcessor&);
    ~ViolentGlitchEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateDisplay();

    ViolentGlitchProcessor& audioProcessor;

    ehl::juce_design::LookAndFeel lookAndFeel;
    ehl::juce_design::ParameterDisplay display{ehl::juce_design::DisplayKind::bitcrusher};
    
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
