#pragma once
#include <JuceHeader.h>

#include <array>

class ViolentGlitchProcessor : public juce::AudioProcessor
{
public:
    ViolentGlitchProcessor();
    ~ViolentGlitchProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

private:
    struct SampleHoldState
    {
        float phase = 0.0f;
        float held = 0.0f;
    };

    std::atomic<float>* crushParam = nullptr;
    std::atomic<float>* rateParam = nullptr;
    std::atomic<float>* chaosParam = nullptr;
    std::atomic<float>* mixParam = nullptr;

    std::array<SampleHoldState, 2> sampleHoldStates {};
    juce::Random random;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ViolentGlitchProcessor)
};
