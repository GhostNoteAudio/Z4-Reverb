#pragma once

#include <JuceHeader.h>
#include <functional>
#include "PluginProcessor.h"
#include "RotaryKnobLive.h"
#include "Meter.h"
#include "LayoutUtils.h"
#include "AboutDialog.h"
#include "Spinner.h"
#include "Stylers.h"
#include "SpinToggle.h"
#include "Presets.h"
#include "InputDialog.h"
#include "RoundToggle.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
const int ResizeTabSize = 24;

class Z4ReverbAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    Z4ReverbAudioProcessorEditor(Z4ReverbAudioProcessor&);
    ~Z4ReverbAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void visibilityChanged() override { applyEditorScaling(); }
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& ev) override;
    void mouseExit(const juce::MouseEvent& ev) override;
    void timerCallback() override;

private:
    void attachParam(juce::Slider* component, int parameter);
    void attachParam(juce::ToggleButton* component, int parameter);
    void addKnobLabels();
    void showAboutDialog();
    void reloadPresetName();
    void savePreset();
    void applyEditorScaling();

    Z4ReverbAudioProcessor& audioProcessor;
    int currentParameter;
    juce::CriticalSection objectLock;
    std::map<juce::Component*, int> componentToParam;
        
    Spinner spinnersParams[3];
    SpinToggleButton spinToggleButtons[1];
    juce::Label spinnerLabels[4];
    juce::Label freezeLabel;

    RotaryKnobLive knobs[12];
    juce::Label knobLabels[12];

    RoundToggleButton freezeButton;
    
    std::unique_ptr<SliderAttachment> attachments[Parameter::COUNT];
    std::unique_ptr<ButtonAttachment> buttonAttachments[Parameter::COUNT];
    
    InputDialog dialog;
    juce::Label pluginName;
    juce::Label parameterReadout;
    juce::Label presetName;
    ModalOverlay overlay;
    AboutDialog aboutDialog;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Z4ReverbAudioProcessorEditor)
};
