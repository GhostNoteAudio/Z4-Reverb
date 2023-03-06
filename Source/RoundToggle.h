#pragma once

#include <JuceHeader.h>
#include "LayoutUtils.h"

class RoundToggleButton : public juce::ToggleButton
{
    bool mouseOver = false;

public:

    enum ColourIds
    {
        fillColourId = 0x200003
    };

    inline RoundToggleButton() : juce::ToggleButton("")
    {
    }

    inline ~RoundToggleButton() override
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
        auto toggled = getToggleState();
        auto colour = findColour(RoundToggleButton::ColourIds::fillColourId);
        colour = colour.brighter(mouseOver ? 0.3f : 0.0f);
        auto lineWidth = getWidth() / 15.0;

        auto box = getLocalBounds().toFloat();
        box.reduce(lineWidth/2 + 1, lineWidth/2 + 1);

        g.setColour(colour);
        g.drawEllipse(box, lineWidth);

        g.setColour(colour.withMultipliedAlpha(toggled ? 1.5 : 0.3f));
        box.reduce(lineWidth * 1.5, lineWidth * 1.5);
        g.fillEllipse(box);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoundToggleButton)
};