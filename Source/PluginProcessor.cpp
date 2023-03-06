/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Authentication.h"

juce::String Authentication::AuthFileName = juce::String(JucePlugin_Desc) + ".auth";
juce::String Authentication::AuthSalt = "";
bool Authentication::isAuthenticatedCacheSet = false;
bool Authentication::isAuthenticatedCacheValue = false;
bool Authentication::isFreePlugin = true;

template<typename T>
inline void Copy(T* dest, const T* source, int len)
{
    memcpy(dest, source, len * sizeof(T));
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(juce::AudioProcessorParameter::Listener* listener)
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    juce::NormalisableRange<float> range = { 0.0f, 1.0f, 0.0001f };

    for (int i = 0; i < Parameter::COUNT; i++)
    {    
        auto para = std::make_unique<juce::AudioParameterFloat>
        (
            ParameterIds[i], // parameterID
            ParameterLabel[i], // parameter name
            range, // min, max, delta
            0.0, // default value
            "", // label
            juce::AudioProcessorParameter::genericParameter,
            [i](float value, int maxlen) { return FormatParameter(value, maxlen, i); } // formatter
        );
        para->addListener(listener);
        layout.add(std::move(para));
    }

    return layout;
}

Z4ReverbAudioProcessor::Z4ReverbAudioProcessor() :
    AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    parameters(*this, nullptr, juce::Identifier(juce::String(JucePlugin_Desc)), createParameterLayout(this)),
    reverbLeft(48000, false), reverbRight(48000, true)
{
    presetName = "-----";
    //Presets::loadPreset(this, "Basic Reverb");
}

Z4ReverbAudioProcessor::~Z4ReverbAudioProcessor()
{
}

const juce::String Z4ReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Z4ReverbAudioProcessor::acceptsMidi() const
{
    return false;
}

bool Z4ReverbAudioProcessor::producesMidi() const
{
    return false;
}

bool Z4ReverbAudioProcessor::isMidiEffect() const
{
    return false;
}

double Z4ReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Z4ReverbAudioProcessor::getNumPrograms()
{
    return 1;
}

int Z4ReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Z4ReverbAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String Z4ReverbAudioProcessor::getProgramName(int index)
{
    return presetName;
}

void Z4ReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void Z4ReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (reverbLeft.GetSamplerate() != sampleRate)
    {
        reverbLeft.SetSamplerate(sampleRate);
        reverbRight.SetSamplerate(sampleRate);
    }
}

void Z4ReverbAudioProcessor::releaseResources()
{
    
}

bool Z4ReverbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    auto numInsMain = layouts.getNumChannels(true, 0);
    auto numOuts = layouts.getNumChannels(false, 0);
    return numInsMain == 2 && numOuts == 2;
}

void Z4ReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int bufSize = buffer.getNumSamples();
    float* inputL = (float*)buffer.getReadPointer(0);
    float* inputR = (float*)buffer.getReadPointer(1);
    float* outputL = buffer.getWritePointer(0);
    float* outputR = buffer.getWritePointer(1);
    float dummyBuffer[32768];

    if (reverbLeft.IsTrueStereo())
    {
        reverbLeft.Process(inputL,       dummyBuffer, outputL,     dummyBuffer, bufSize);
        reverbRight.Process(dummyBuffer, inputR,      dummyBuffer, outputR,     bufSize);
    }
    else
    {
        reverbLeft.Process(inputL, inputR, outputL, outputR, bufSize);
    }
}

bool Z4ReverbAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* Z4ReverbAudioProcessor::createEditor()
{ 
    return new Z4ReverbAudioProcessorEditor(*this);
}

void Z4ReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    
    auto presetName = new juce::XmlElement("PresetName");
    presetName->addTextElement(getPresetName());
    xml->addChildElement(presetName);
    auto str = xml->toString({});
    copyXmlToBinary(*xml, destData); 
}

void Z4ReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    auto str = xmlState->toString({});

    auto presetNameElement = xmlState->getChildByName("PresetName");
    auto presetName = presetNameElement->getAllSubText();
    xmlState->removeChildElement(presetNameElement, true);

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));

    setPresetName(presetName);
    reapplyAllParams();
}

void Z4ReverbAudioProcessor::parameterValueChanged(int idx, float value)
{
    reverbLeft.SetParameter(idx, value);
    reverbRight.SetParameter(idx, value);
}

void Z4ReverbAudioProcessor::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{

}

void Z4ReverbAudioProcessor::reapplyAllParams()
{
    for (int i = 0; i < Parameter::COUNT; i++)
    {
        reverbLeft.SetParameter(i, getParamByIdx(i));
        reverbRight.SetParameter(i, getParamByIdx(i));
    }
}

float Z4ReverbAudioProcessor::getParamByIdx(int idx)
{
    return parameters.getParameter(ParameterIds[idx])->getValue();
}

void Z4ReverbAudioProcessor::setPresetName(juce::String presetName)
{
    this->presetName = presetName;
}

juce::String Z4ReverbAudioProcessor::getPresetName()
{
    return this->presetName;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Z4ReverbAudioProcessor();
}
