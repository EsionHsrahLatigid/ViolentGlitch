#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
void styleSlider(juce::Slider& slider, const juce::String& name)
{
    slider.setName(name);
    ehl::juce_design::styleSlider(slider);
}

void styleLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    ehl::juce_design::styleLabel(label);
    label.setJustificationType(juce::Justification::centred);
}
} // namespace

ViolentGlitchEditor::ViolentGlitchEditor(ViolentGlitchProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&lookAndFeel);
    
    styleSlider(crushSlider, "Bit Crush");
    addAndMakeVisible(crushSlider);
    crushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "crush", crushSlider);
    styleLabel(crushLabel, "BIT CRUSH");
    addAndMakeVisible(crushLabel);
    
    styleSlider(rateSlider, "Destroy");
    addAndMakeVisible(rateSlider);
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "rate", rateSlider);
    styleLabel(rateLabel, "DESTROY");
    addAndMakeVisible(rateLabel);
    
    styleSlider(chaosSlider, "Chaos");
    addAndMakeVisible(chaosSlider);
    chaosAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "chaos", chaosSlider);
    styleLabel(chaosLabel, "CHAOS");
    addAndMakeVisible(chaosLabel);
    
    styleSlider(mixSlider, "Mix");
    addAndMakeVisible(mixSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "mix", mixSlider);
    styleLabel(mixLabel, "MIX");
    addAndMakeVisible(mixLabel);
    addAndMakeVisible(display);

    setResizable(true, true);
    setResizeLimits(ehl::juce_design::Metrics::minimumWidth,
                    ehl::juce_design::Metrics::minimumHeight,
                    ehl::juce_design::Metrics::maximumWidth,
                    ehl::juce_design::Metrics::maximumHeight);
    setSize(ehl::juce_design::Metrics::defaultWidth,
            ehl::juce_design::Metrics::defaultHeight);
    updateDisplay();
    startTimerHz(15);
}

ViolentGlitchEditor::~ViolentGlitchEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void ViolentGlitchEditor::paint(juce::Graphics& g)
{
    ehl::juce_design::paintEditorChrome(g, getLocalBounds(),
                                        "ViolentGlitch",
                                        "destructive bit / rate corruption");
}

void ViolentGlitchEditor::resized()
{
    display.setBounds(ehl::juce_design::parameterDisplayArea(getLocalBounds()));
    ehl::juce_design::layoutLabelledControl(
        crushLabel, crushSlider, ehl::juce_design::controlCell(getLocalBounds(), 0));
    ehl::juce_design::layoutLabelledControl(
        rateLabel, rateSlider, ehl::juce_design::controlCell(getLocalBounds(), 1));
    ehl::juce_design::layoutLabelledControl(
        chaosLabel, chaosSlider, ehl::juce_design::controlCell(getLocalBounds(), 2));
    ehl::juce_design::layoutLabelledControl(
        mixLabel, mixSlider, ehl::juce_design::controlCell(getLocalBounds(), 3));
}

void ViolentGlitchEditor::timerCallback() { updateDisplay(); }

void ViolentGlitchEditor::updateDisplay()
{
    const auto normalized = [this](const char* id)
    {
        if (auto* parameter = audioProcessor.apvts.getParameter(id))
            return parameter->getValue();
        return 0.0f;
    };
    display.setValues({ normalized("crush"), normalized("rate"),
                        normalized("chaos"), normalized("mix") });
}
