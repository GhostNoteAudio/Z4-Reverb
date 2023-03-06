#pragma once

#include <JuceHeader.h>
#include "LayoutUtils.h"

class SpinToggleButton: public juce::ToggleButton
{
    bool mouseOver = false;

    juce::String getLabel()
    {
        auto toggled = getToggleState();
        return juce::String(toggled ? OnValue : OffValue);
    }

public:
    juce::String OffValue;
    juce::String OnValue;

    inline SpinToggleButton() : juce::ToggleButton("-")
    {
        OffValue = "Off";
        OnValue = "On";
    }

    inline ~SpinToggleButton() override
    {
    }

    inline void mouseExit(const juce::MouseEvent& ev) override
    {
        mouseOver = false;
        juce::ToggleButton::mouseExit(ev);
    }

    inline void mouseEnter(const juce::MouseEvent& ev) override
    {
        mouseOver = true;
        juce::ToggleButton::mouseEnter(ev);
    }

    inline void paintButton(juce::Graphics& g, bool highlighted, bool down) override
    {
        auto colour = findColour(Spinner::ColourIds::backgroundColourId);
        double scale = getHeight() / 24.0;
        g.setColour(colour);
        g.fillRoundedRectangle(1, 1, getWidth() - 2, getHeight() - 2, 4);

        colour = findColour(Spinner::ColourIds::textColourId);
        if (mouseOver) colour = colour.brighter(0.3f);
        g.setColour(colour);
        g.setFont(getFontRegular(20 * scale));
        g.drawText(getLabel(), 2, 0, getWidth(), getHeight(), juce::Justification::centred);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpinToggleButton)
};