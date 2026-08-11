#include "PluginProcessor.h"

#include <juce_events/juce_events.h>
#include <cmath>
#include <iostream>

namespace
{
bool check(bool condition, const char* message)
{
    if (!condition)
        std::cerr << "[FAIL] " << message << '\n';
    return condition;
}

void setParameter(juce::AudioProcessorValueTreeState& state, const char* id, float value)
{
    if (auto* parameter = state.getParameter(id))
        parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
}

bool checkNear(float actual, float expected, float tolerance, const char* message)
{
    return check(std::abs(actual - expected) <= tolerance, message);
}
} // namespace

int main()
{
    juce::ScopedJuceInitialiser_GUI initialiseJuce;
    ViolentGlitchProcessor processor;
    bool passed = true;

    passed &= check(processor.getName() == "ViolentGlitch", "product name should be ViolentGlitch");
    passed &= check(!processor.acceptsMidi(), "processor should not accept MIDI");
    passed &= check(!processor.isMidiEffect(), "processor should be an audio effect");

    juce::AudioProcessor::BusesLayout stereo;
    stereo.inputBuses.add(juce::AudioChannelSet::stereo());
    stereo.outputBuses.add(juce::AudioChannelSet::stereo());
    passed &= check(processor.isBusesLayoutSupported(stereo), "stereo input/output should be supported");

    auto* mix = processor.apvts.getParameter("mix");
    passed &= check(mix != nullptr, "Mix parameter should exist");
    if (mix != nullptr)
    {
        mix->setValueNotifyingHost(mix->convertTo0to1(0.25f));
        juce::MemoryBlock state;
        processor.getStateInformation(state);
        mix->setValueNotifyingHost(mix->convertTo0to1(0.75f));
        processor.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        passed &= check(std::abs(processor.apvts.getRawParameterValue("mix")->load() - 0.25f) < 0.001f,
                        "APVTS state should round-trip");
    }

    constexpr double sampleRate = 44100.0;
    processor.prepareToPlay(sampleRate, 512);

    for (int block = 0; block < 8; ++block)
    {
        juce::AudioBuffer<float> audio(2, 512);
        for (int sample = 0; sample < audio.getNumSamples(); ++sample)
        {
            const auto value = static_cast<float>(0.2 * std::sin(2.0 * juce::MathConstants<double>::pi
                                                                 * 220.0 * sample / sampleRate));
            audio.setSample(0, sample, value);
            audio.setSample(1, sample, value);
        }

        juce::MidiBuffer midi;
        processor.processBlock(audio, midi);

        for (int channel = 0; channel < audio.getNumChannels(); ++channel)
            for (int sample = 0; sample < audio.getNumSamples(); ++sample)
                passed &= check(std::isfinite(audio.getSample(channel, sample)), "processed audio should remain finite");
    }

    setParameter(processor.apvts, "crush", 16.0f);
    setParameter(processor.apvts, "rate", 0.25f);
    setParameter(processor.apvts, "chaos", 0.0f);
    setParameter(processor.apvts, "mix", 1.0f);
    processor.prepareToPlay(sampleRate, 512);

    juce::AudioBuffer<float> identicalStereo(2, 128);
    for (int sample = 0; sample < identicalStereo.getNumSamples(); ++sample)
    {
        const auto value = static_cast<float>((sample % 17) / 17.0 - 0.5);
        identicalStereo.setSample(0, sample, value);
        identicalStereo.setSample(1, sample, value);
    }

    juce::MidiBuffer midi;
    processor.processBlock(identicalStereo, midi);
    for (int sample = 0; sample < identicalStereo.getNumSamples(); ++sample)
        passed &= checkNear(identicalStereo.getSample(0, sample), identicalStereo.getSample(1, sample), 1.0e-6f,
                            "matching stereo channels should not diverge through shared sample-hold state");

    ViolentGlitchProcessor wholeBlockProcessor;
    ViolentGlitchProcessor splitBlockProcessor;
    setParameter(wholeBlockProcessor.apvts, "crush", 16.0f);
    setParameter(wholeBlockProcessor.apvts, "rate", 0.25f);
    setParameter(wholeBlockProcessor.apvts, "chaos", 0.0f);
    setParameter(wholeBlockProcessor.apvts, "mix", 1.0f);
    setParameter(splitBlockProcessor.apvts, "crush", 16.0f);
    setParameter(splitBlockProcessor.apvts, "rate", 0.25f);
    setParameter(splitBlockProcessor.apvts, "chaos", 0.0f);
    setParameter(splitBlockProcessor.apvts, "mix", 1.0f);
    wholeBlockProcessor.prepareToPlay(sampleRate, 512);
    splitBlockProcessor.prepareToPlay(sampleRate, 512);

    juce::AudioBuffer<float> wholeBlock(2, 512);
    juce::AudioBuffer<float> splitBlock(2, 512);
    for (int sample = 0; sample < wholeBlock.getNumSamples(); ++sample)
    {
        const auto left = static_cast<float>(0.3 * std::sin(2.0 * juce::MathConstants<double>::pi
                                                            * 73.0 * sample / sampleRate));
        const auto right = static_cast<float>(0.2 * std::cos(2.0 * juce::MathConstants<double>::pi
                                                             * 137.0 * sample / sampleRate));
        wholeBlock.setSample(0, sample, left);
        wholeBlock.setSample(1, sample, right);
        splitBlock.setSample(0, sample, left);
        splitBlock.setSample(1, sample, right);
    }

    wholeBlockProcessor.processBlock(wholeBlock, midi);

    juce::AudioBuffer<float> splitFirst(2, 193);
    juce::AudioBuffer<float> splitSecond(2, 319);
    for (int channel = 0; channel < 2; ++channel)
    {
        splitFirst.copyFrom(channel, 0, splitBlock, channel, 0, splitFirst.getNumSamples());
        splitSecond.copyFrom(channel, 0, splitBlock, channel, splitFirst.getNumSamples(), splitSecond.getNumSamples());
    }

    splitBlockProcessor.processBlock(splitFirst, midi);
    splitBlockProcessor.processBlock(splitSecond, midi);
    for (int channel = 0; channel < 2; ++channel)
    {
        splitBlock.copyFrom(channel, 0, splitFirst, channel, 0, splitFirst.getNumSamples());
        splitBlock.copyFrom(channel, splitFirst.getNumSamples(), splitSecond, channel, 0, splitSecond.getNumSamples());
    }

    for (int channel = 0; channel < 2; ++channel)
        for (int sample = 0; sample < wholeBlock.getNumSamples(); ++sample)
            passed &= checkNear(splitBlock.getSample(channel, sample), wholeBlock.getSample(channel, sample), 1.0e-6f,
                                "split-block processing should match whole-block processing");

    if (passed)
        std::cout << "ViolentGlitch plug-in integration checks passed\n";
    return passed ? 0 : 1;
}
