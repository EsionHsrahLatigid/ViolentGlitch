#include "PluginProcessor.h"
#include "PluginEditor.h"

ViolentGlitchEditor::ViolentGlitchEditor(ViolentGlitchProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 300);
    
    // Bit Crush slider
    crushSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    crushSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(crushSlider);
    crushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "crush", crushSlider);
    crushLabel.setText("BIT CRUSH", juce::dontSendNotification);
    crushLabel.setJustificationType(juce::Justification::centred);
    crushLabel.attachToComponent(&crushSlider, false);
    addAndMakeVisible(crushLabel);
    
    // Sample Rate slider
    rateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(rateSlider);
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "rate", rateSlider);
    rateLabel.setText("DESTROY", juce::dontSendNotification);
    rateLabel.setJustificationType(juce::Justification::centred);
    rateLabel.attachToComponent(&rateSlider, false);
    addAndMakeVisible(rateLabel);
    
    // Chaos slider
    chaosSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    chaosSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(chaosSlider);
    chaosAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "chaos", chaosSlider);
    chaosLabel.setText("CHAOS", juce::dontSendNotification);
    chaosLabel.setJustificationType(juce::Justification::centred);
    chaosLabel.attachToComponent(&chaosSlider, false);
    addAndMakeVisible(chaosLabel);
    
    // Mix slider
    mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(mixSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "mix", mixSlider);
    mixLabel.setText("MIX", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.attachToComponent(&mixSlider, false);
    addAndMakeVisible(mixLabel);
}

ViolentGlitchEditor::~ViolentGlitchEditor() {}

void ViolentGlitchEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    
    g.setColour(juce::Colours::red);
    g.setFont(24.0f);
    g.drawFittedText("VIOLENT GLITCH", getLocalBounds().removeFromTop(40), 
                     juce::Justification::centred, 1);
}

void ViolentGlitchEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(40);
    
    auto sliderBounds = bounds.removeFromTop(180);
    auto sliderWidth = sliderBounds.getWidth() / 4;
    
    crushSlider.setBounds(sliderBounds.removeFromLeft(sliderWidth).reduced(10));
    rateSlider.setBounds(sliderBounds.removeFromLeft(sliderWidth).reduced(10));
    chaosSlider.setBounds(sliderBounds.removeFromLeft(sliderWidth).reduced(10));
    mixSlider.setBounds(sliderBounds.removeFromLeft(sliderWidth).reduced(10));
}
