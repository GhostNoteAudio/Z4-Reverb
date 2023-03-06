#pragma once

#include <JuceHeader.h>
#include "Stylers.h"

class RoundedPanel  : public juce::Component
{
public:
    inline RoundedPanel()
    {
    }

    inline ~RoundedPanel() override
    {
    }

	inline void paint (juce::Graphics& g) override
    {
		g.setColour(ColourMenuBackground);
        g.fillRoundedRectangle(lineThickness / 2, lineThickness / 2, getWidth() - lineThickness, getHeight() - lineThickness, cornerRadius);
		if (lineThickness > 0)
		{
			g.setColour(lineColour);
			g.drawRoundedRectangle(lineThickness/2, lineThickness/2, getWidth() - lineThickness, getHeight() - lineThickness, cornerRadius, lineThickness);
		}
    }

	inline void resized() override
    {
    }

	juce::Colour lineColour = ColourMenuText;
	double lineThickness = 2.0;
	double cornerRadius = 10.0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RoundedPanel)
};
