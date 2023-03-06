#pragma once

#include <functional>
#include <JuceHeader.h>


class Spinner : public juce::Slider
{
	bool mouseOver = false;

public:
	enum ColourIds
	{
		backgroundColourId = 0x200001,
		textColourId = 0x200002,
	};

	std::function<juce::String(double)> Formatter;

	inline Spinner()
	{
		setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 70, 25);
		setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
		setRange(0.0, 1.0);

		Formatter = [](double val) {return juce::String(val); };
	}

	inline ~Spinner() override
	{
	}

	inline void mouseExit(const juce::MouseEvent& ev) override
	{
		mouseOver = false;
		juce::Slider::mouseExit(ev);
	}

	inline void mouseEnter(const juce::MouseEvent& ev) override
	{
		mouseOver = true;
		juce::Slider::mouseEnter(ev);
	}

	inline void paint(juce::Graphics& g) override
	{
		auto relValue = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
		auto colour = findColour(Spinner::ColourIds::backgroundColourId);
		double scale = getHeight() / 24.0;
		g.setColour(colour);
		g.fillRoundedRectangle(1, 1, getWidth() - 2, getHeight() - 2, 4);

		colour = findColour(Spinner::ColourIds::textColourId);
		if (mouseOver) colour = colour.brighter(0.3f);
		g.setColour(colour);
		g.setFont(getFontRegular(20 * scale));
		g.drawText(Formatter(relValue), 2, 0, getWidth(), getHeight(), juce::Justification::centred);
	}

	double lineThickness = 5;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spinner)
};
