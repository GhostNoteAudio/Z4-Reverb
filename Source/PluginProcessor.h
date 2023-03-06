#pragma once

#include <JuceHeader.h>
#include "Parameters.h"
#include "DSP/Z4Rev.h"

//==============================================================================
/**
*/
class Z4ReverbAudioProcessor : public juce::AudioProcessor, public juce::AudioProcessorParameter::Listener
{
public:
    //==============================================================================
    Z4ReverbAudioProcessor();
    ~Z4ReverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;

    void reapplyAllParams();
    float getParamByIdx(int idx);
    void setPresetName(juce::String presetName);
    juce::String getPresetName();

    juce::AudioProcessorValueTreeState parameters;
    Z4::Z4Rev reverbLeft;
    Z4::Z4Rev reverbRight;
private:
    
    juce::String presetName;
    float bufL[32768] = { 0 };
    float bufR[32768] = { 0 };
    float bufS[32768] = { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Z4ReverbAudioProcessor)
};
