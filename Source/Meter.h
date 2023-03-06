#pragma once

#include <JuceHeader.h>

class Meter : public juce::Component
{
public:
	inline Meter()
	{
		value = 0.5;
	}

	inline ~Meter() override
	{
	}

	inline void paint(juce::Graphics& g) override
	{
		float w = getWidth();
		float h = getHeight();
		g.setColour(findColour(juce::Slider::trackColourId));
		g.fillRoundedRectangle(0, 0, w, h, 0);

		if (value >= 0)
		{
			g.setColour(findColour(juce::Slider::thumbColourId));
			g.fillRoundedRectangle(0, (1 - value) * h, w, h * value, 0);
		}
		else
		{
			g.setColour(findColour(juce::Slider::thumbColourId));
			g.fillRoundedRectangle(0, 0, w, h * -value, 0);
		}
	}

	inline void setValue(float value)
	{
		this->value = value;
		repaint();
	}

private:
	float value;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Meter)
};
