#include "PluginProcessor.h"
#include "PluginEditor.h"

ViolentGlitchProcessor::ViolentGlitchProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    crushParam = apvts.getRawParameterValue("crush");
    rateParam = apvts.getRawParameterValue("rate");
    chaosParam = apvts.getRawParameterValue("chaos");
    mixParam = apvts.getRawParameterValue("mix");
}

ViolentGlitchProcessor::~ViolentGlitchProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout ViolentGlitchProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "crush", "Bit Crush", 1.0f, 16.0f, 8.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "rate", "Sample Destroy", 0.01f, 1.0f, 0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "chaos", "Digital Chaos", 0.0f, 1.0f, 0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "mix", "Mix", 0.0f, 1.0f, 1.0f));
    
    return layout;
}

const juce::String ViolentGlitchProcessor::getName() const { return JucePlugin_Name; }
bool ViolentGlitchProcessor::acceptsMidi() const { return false; }
bool ViolentGlitchProcessor::producesMidi() const { return false; }
bool ViolentGlitchProcessor::isMidiEffect() const { return false; }
double ViolentGlitchProcessor::getTailLengthSeconds() const { return 0.0; }
int ViolentGlitchProcessor::getNumPrograms() { return 1; }
int ViolentGlitchProcessor::getCurrentProgram() { return 0; }
void ViolentGlitchProcessor::setCurrentProgram(int) {}
const juce::String ViolentGlitchProcessor::getProgramName(int) { return {}; }
void ViolentGlitchProcessor::changeProgramName(int, const juce::String&) {}

void ViolentGlitchProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    for (auto& state : sampleHoldStates)
        state = {};
}

void ViolentGlitchProcessor::releaseResources() {}

bool ViolentGlitchProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void ViolentGlitchProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const float crush = crushParam->load();
    const float rate = rateParam->load();
    const float chaos = chaosParam->load();
    const float mix = mixParam->load();
    
    const float step = std::pow(2.0f, crush);
    const float chaosAmount = chaos * 0.95f;
    
    const auto channelsToProcess = juce::jmin(totalNumInputChannels, static_cast<int>(sampleHoldStates.size()));

    for (int channel = 0; channel < channelsToProcess; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto& sampleHold = sampleHoldStates[static_cast<std::size_t>(channel)];
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float input = channelData[sample];
            float output = input;
            
            // Violent bit crushing
            output = std::floor(output * step) / step;
            
            // Sample rate destruction with chaos
            sampleHold.phase += rate;
            if (sampleHold.phase >= 1.0f)
            {
                sampleHold.phase -= 1.0f;
                sampleHold.held = output;
                
                // Digital chaos - random bit flips and sign inversions
                if (random.nextFloat() < chaosAmount)
                {
                    sampleHold.held *= -1.0f;
                }
                
                if (random.nextFloat() < chaosAmount * 0.5f)
                {
                    sampleHold.held += (random.nextFloat() - 0.5f) * 2.0f * chaosAmount;
                }
                
                // Harsh digital wrap/fold
                if (random.nextFloat() < chaosAmount * 0.3f)
                {
                    sampleHold.held = std::fmod(sampleHold.held * 10.0f, 2.0f) - 1.0f;
                }
            }
            
            output = sampleHold.held;
            
            // Hard digital clipping
            output = juce::jlimit(-1.0f, 1.0f, output * (1.0f + chaos * 2.0f));
            
            // Additional glitch layer
            if (random.nextFloat() < chaos * 0.1f)
            {
                output = random.nextFloat() < 0.5f ? 1.0f : -1.0f;
            }
            
            // Mix
            channelData[sample] = input * (1.0f - mix) + output * mix;
        }
    }
}

bool ViolentGlitchProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* ViolentGlitchProcessor::createEditor()
{
    return new ViolentGlitchEditor(*this);
}

void ViolentGlitchProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ViolentGlitchProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ViolentGlitchProcessor();
}
