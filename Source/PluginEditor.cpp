/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "LayoutUtils.h"

const int PanelPadding = 20;
const int KnobSpacing = 122;

const int PluginWidth = 1116;
const int PluginHeight = 700;
const int Panelx = 172;
const int Panelw = PluginWidth - 2 * Panelx;
const int Panely = 278;
const int Panelh = 273;

// global variable, affects all plugin instances, is retained until process exits
bool pluginScaleLarge = true;

//==============================================================================
Z4ReverbAudioProcessorEditor::Z4ReverbAudioProcessorEditor(Z4ReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    const juce::ScopedLock myScopedLock(objectLock);
    currentParameter = Parameter::COUNT;
    setSize(PluginWidth, PluginHeight);
    
    setLookAndFeel(&CustomLaf);

    parameterReadout.setFont(getFontLight(36));
    parameterReadout.setText(" ", juce::NotificationType::dontSendNotification);
    parameterReadout.setJustificationType(juce::Justification::centredRight);
    parameterReadout.setColour(juce::Label::ColourIds::textColourId, ColourLabels);
    parameterReadout.setBounds(PluginWidth - 420, 635, 400, 32);
    addAndMakeVisible(parameterReadout);

    presetName.setFont(getFontLight(42));
    presetName.setText(this->audioProcessor.getPresetName(), juce::NotificationType::dontSendNotification);
    presetName.setJustificationType(juce::Justification::centred);
    presetName.setColour(juce::Label::ColourIds::textColourId, ColourLabels);
    presetName.setBounds(PluginWidth / 2 - 300, 626, 600, 50);
    presetName.addMouseListener(this, false);
    presetName.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(presetName);
    
    int x0 = Panelx + PanelPadding;
    int y0 = 296;
    int y1 = 430;
    int w = KnobSpacing;
    int h = 70;
    int i = 0;

    for (int i = 0; i < 12; i++)
    {
        int x = x0 + (i / 2) * w;
        int y = i % 2 == 0 ? y0 : y1;
        knobs[i].setBounds(x, y, w, h);
        addAndMakeVisible(knobs[i]);
        attachParam(&knobs[i], Parameter::Dry + i);
    }

    addKnobLabels();

    int sw = 85; // spinner width
    int sh = 28; // spinner height
    int sy = 190; // spinner y pos

    spinnersParams[0].setBounds(96, sy, sw, sh);
    attachParam(&spinnersParams[0], Parameter::Stages);
    spinnersParams[0].Formatter = [](double val) { return FormatParameter(val, 32, Parameter::Stages); };

    spinnersParams[1].setBounds(272, sy, sw, sh);
    attachParam(&spinnersParams[1], Parameter::Shimmer);
    spinnersParams[1].Formatter = [](double val) { return FormatParameter(val, 32, Parameter::Shimmer); };

    spinnersParams[2].setBounds(PluginWidth - 96 - sw, sy, sw, sh);
    attachParam(&spinnersParams[2], Parameter::Saturate);
    spinnersParams[2].Formatter = [](double val) { return FormatParameter(val, 32, Parameter::Saturate); };
    spinnersParams[2].setMouseDragSensitivity(100);

    spinnersParams[0].setMouseCursor(juce::MouseCursor::BottomEdgeResizeCursor);
    spinnersParams[1].setMouseCursor(juce::MouseCursor::BottomEdgeResizeCursor);
    spinnersParams[2].setMouseCursor(juce::MouseCursor::BottomEdgeResizeCursor);

    addAndMakeVisible(spinnersParams[0]);
    addAndMakeVisible(spinnersParams[1]);
    addAndMakeVisible(spinnersParams[2]);
    
    spinToggleButtons[0].setBounds(PluginWidth - 272 - sw, sy, sw, sh);
    attachParam(&spinToggleButtons[0], Parameter::Stereo);
    addAndMakeVisible(spinToggleButtons[0]);
    spinToggleButtons[i].setMouseCursor(juce::MouseCursor::PointingHandCursor);

    for (int i = 0; i < 4; i++)
    {
        styleSpinnerLabel(&spinnerLabels[i]);
        spinnerLabels[i].setText(ParameterLabel[Parameter::Stages + i], juce::NotificationType::dontSendNotification);
        addAndMakeVisible(spinnerLabels[i]);
    }

    spinnerLabels[0].setBounds(96 - 20, sy - 40, sw + 40, 30);
    spinnerLabels[1].setBounds(272 - 20, sy - 40, sw + 40, 30);
    spinnerLabels[2].setBounds(PluginWidth - 272 - sw - 20, sy - 40, sw + 40, 30);
    spinnerLabels[3].setBounds(PluginWidth - 96 - sw - 20, sy - 40, sw + 40, 30);

    freezeButton.setBounds(PluginWidth / 2 - 30, sy - 5, 60, 60);
    attachParam(&freezeButton, Parameter::Freeze);
    addAndMakeVisible(freezeButton);

    styleSpinnerLabel(&freezeLabel);
    freezeLabel.setFont(getFontLight(48));
    freezeLabel.setText(ParameterLabel[Parameter::Freeze], juce::NotificationType::dontSendNotification);
    addAndMakeVisible(freezeLabel);
    freezeLabel.setBounds(PluginWidth/2 - 120, sy - 58, 240, 40);

    pluginName.setText("Z4 Reverb", juce::NotificationType::dontSendNotification);
    pluginName.setFont(getFontLight(70));
    pluginName.setJustificationType(juce::Justification::centredTop);
    pluginName.setColour(juce::Label::ColourIds::textColourId, ColourTitle);
    pluginName.setBounds(PluginWidth/2 - 300, 10, 600, 70);
    addAndMakeVisible(pluginName);

    aboutDialog.setBounds(0, 0, 550, 180);
    aboutDialog.SetOverlay(&overlay);
    overlay.setComponent(&aboutDialog);
    overlay.setBounds(0, 0, PluginWidth, PluginHeight);
    addAndMakeVisible(overlay);
    overlay.setVisible(false);
    
    startTimerHz(30);

    // Adjust the About dialog to compensate for small scaling factor
    overlay.setTransform(juce::AffineTransform::scale(1.5, 1.5, PluginWidth / 2, PluginHeight / 2));
}

Z4ReverbAudioProcessorEditor::~Z4ReverbAudioProcessorEditor()
{
    stopTimer();
    {
        const juce::ScopedLock myScopedLock(objectLock);
        // blocks until timer callback is done
    }
}

void Z4ReverbAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(ColourAlmostWhite);

    juce::ColourGradient grad(juce::Colour::fromRGB(42, 71, 80), 0, 100, juce::Colour::fromRGB(69, 111, 101), 0, 500, false);
    g.setGradientFill(grad);
    g.fillRect(0, 100, PluginWidth, 500);
    drawNoise(g, 0, 100, PluginWidth, 500);

    juce::ColourGradient gradTop(juce::Colour::fromFloatRGBA(0, 0, 0, 0.3), 0, 100, juce::Colour::fromFloatRGBA(0, 0, 0, 0), 0, 205, false);
    g.setGradientFill(gradTop);
    g.fillRect(0, 100, PluginWidth, 105);

    g.setColour(juce::Colour::fromFloatRGBA(1, 1, 1, 0.03));
    g.fillRoundedRectangle(Panelx, Panely, Panelw, Panelh, 3);

    auto img = juce::Drawable::createFromImageData(BinaryData::logo_svg, BinaryData::logo_svgSize);
    img->drawWithin(g, juce::Rectangle<float>(20, 14, 350, 66), juce::RectanglePlacement::xLeft | juce::RectanglePlacement::yTop, 1.0);

    juce::Path p;
    p.startNewSubPath(PluginWidth - ResizeTabSize, PluginHeight);
    p.lineTo(PluginWidth, PluginHeight - ResizeTabSize);
    p.lineTo(PluginWidth, PluginHeight);
    p.closeSubPath();
    g.setColour(juce::Colour::fromHSL(0, 0, 0, 0.15));
    g.fillPath(p);
}

void Z4ReverbAudioProcessorEditor::resized()
{
    overlay.setBounds(0, 0, getWidth(), getHeight());
}

void Z4ReverbAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
    if (event.eventComponent != this)
        return;

    if (event.x < 320 && event.y <= 100)
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    else if (event.x + event.y >= getWidth() + getHeight() - ResizeTabSize)
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    else
        setMouseCursor(juce::MouseCursor::NormalCursor);
}

void Z4ReverbAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if (event.eventComponent == &presetName)
    {
        Presets::showPresetMenu(
            &this->audioProcessor,
            [this]() {this->reloadPresetName(); },
            [this]() {this->savePreset(); }
        );
    }

    if (event.eventComponent != this)
        return;

    if (event.x < 320 && event.y <= 100)
    {
        showAboutDialog();
    }

    if (event.x + event.y >= getWidth() + getHeight() - ResizeTabSize)
    {
        pluginScaleLarge = !pluginScaleLarge;
        applyEditorScaling();
    }
}

void Z4ReverbAudioProcessorEditor::mouseEnter(const juce::MouseEvent& ev)
{
    if (ev.eventComponent == &presetName)
        presetName.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colour::fromRGB(240, 240, 245));
    

    if (componentToParam.count(ev.eventComponent))
    {
        auto param = componentToParam[ev.eventComponent];
        currentParameter = param;
    }
}

void Z4ReverbAudioProcessorEditor::mouseExit(const juce::MouseEvent& ev)
{
    if (ev.eventComponent == &presetName)
        presetName.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::transparentWhite);

    if (componentToParam.count(ev.eventComponent))
    {
        auto param = componentToParam[ev.eventComponent];
        if (currentParameter == param)
        {
            currentParameter = Parameter::COUNT;
        }
    }
}


void Z4ReverbAudioProcessorEditor::timerCallback()
{
    if (currentParameter == Parameter::COUNT)
    {
        parameterReadout.setText("", juce::NotificationType::dontSendNotification);
        return;
    }

    auto value = audioProcessor.getParamByIdx((int)currentParameter);
    juce::String text = ParameterLabel[(int)currentParameter] + juce::String(": ") + FormatParameter(value, 32, (int)currentParameter);
    parameterReadout.setText(text, juce::NotificationType::dontSendNotification);
}

void Z4ReverbAudioProcessorEditor::attachParam(juce::Slider* component, int parameter)
{
    attachments[parameter].reset(new SliderAttachment(audioProcessor.parameters, juce::String(ParameterIds[parameter]), *component));
    component->addMouseListener(this, false);
    componentToParam[component] = parameter;
}

void Z4ReverbAudioProcessorEditor::attachParam(juce::ToggleButton* component, int parameter)
{
    buttonAttachments[parameter].reset(new ButtonAttachment(audioProcessor.parameters, juce::String(ParameterIds[parameter]), *component));
    component->addMouseListener(this, false);
    componentToParam[component] = parameter;
}

void Z4ReverbAudioProcessorEditor::addKnobLabels()
{
    for (int i = 0; i < 12; i++)
    {
        auto knob = &this->knobs[i];
        int param = componentToParam[knob];
        styleKnobLabel(&knobLabels[i]);
        knobLabels[i].setText(ParameterLabel[param], juce::NotificationType::dontSendNotification);
        knobLabels[i].setBounds(knob->getPosition().x - 10, knob->getPosition().y + 74, knob->getWidth() + 20, 30);
        addAndMakeVisible(knobLabels[i]);
    }
}

void Z4ReverbAudioProcessorEditor::showAboutDialog()
{
    overlay.setComponent(&aboutDialog);
    overlay.setVisible(true);
}

void Z4ReverbAudioProcessorEditor::reloadPresetName()
{
    presetName.setText(this->audioProcessor.getPresetName(), juce::NotificationType::dontSendNotification);
}

void Z4ReverbAudioProcessorEditor::savePreset()
{
    auto cbInner = [this](std::string value, int btnIndex)
    {
        if (btnIndex == 0)
        {
            this->audioProcessor.setPresetName(value);
            Presets::savePreset(&this->audioProcessor, value);
            reloadPresetName();
        }

        overlay.setVisible(false);
    };

    std::vector<std::string> buttons;
    buttons.push_back("Save");
    buttons.push_back("Cancel");
    dialog.setContent("Save Preset", buttons, cbInner);
    overlay.setComponent(&dialog);
    overlay.setVisible(true);
}

void Z4ReverbAudioProcessorEditor::applyEditorScaling()
{
    if (pluginScaleLarge)
        setScaleFactor(1.0);
    else
        setScaleFactor(0.75);

    repaint();
}