#pragma once

#include <JuceHeader.h>

class RotaryKnobLive  : public juce::Slider
{
	bool mouseOver = false;

public:
	inline RotaryKnobLive()
    {
		setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 70, 25);
		setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
		setRange(0.0, 1.0);
    }

	inline ~RotaryKnobLive() override
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

	inline void paint (juce::Graphics& g) override
    {
		auto pi = juce::MathConstants<double>::pi;
		auto sliderPos = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
		auto nominalWidth = 100.0;
		auto nomninalHeight = 100.0;
		auto scaleW = getWidth() / nominalWidth;
		auto scaleH = getHeight() / nomninalHeight;
		auto scale = juce::jmin(scaleW, scaleH);
		auto marginLeft = juce::jmax(getWidth() - scale * nominalWidth, 0.0) / 2;
		g.addTransform(juce::AffineTransform::scale(scale));
		g.addTransform(juce::AffineTransform::translation((float)marginLeft / scale, 0.0f));

		auto radius = nominalWidth / 2 - lineThickness / 2 - 1;
		auto centreX = nominalWidth * 0.5;
		auto centreY = nominalWidth * 0.5;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0;
		auto pathAngle = (-0.75 + 1.5 * sliderPos) * pi;
		auto tickRangeAdjuster = lineThickness / 300.0;
		auto tickAngle = (-(0.25 - tickRangeAdjuster) + (1.5 - tickRangeAdjuster*2) * sliderPos) * pi;
		juce::Path p;

		auto colour = findColour(juce::Slider::rotarySliderFillColourId);
		g.setColour(colour);
		//g.fillEllipse(rx - lineThickness / 2 + 0.5, ry - lineThickness / 2 + 0.5, 2*radius + lineThickness - 1, 2*radius + lineThickness - 1);
		g.fillEllipse(1.2, 1.2, nominalWidth - 2.4, nomninalHeight - 2.4);

		colour = findColour(juce::Slider::trackColourId);
		g.setColour(colour);
		p.addArc(rx, ry, rw, rw, pathAngle, 0.75 * pi, true);
		g.strokePath(p, juce::PathStrokeType(lineThickness));
		p.clear();

		colour = findColour(juce::Slider::thumbColourId);
		if (mouseOver) colour = colour.brighter(0.3f);
		g.setColour(colour);
		p.addArc(rx, ry, rw, rw, -0.75 * pi, pathAngle, true);
		g.strokePath(p, juce::PathStrokeType(lineThickness));
		p.clear();
		
		p.addLineSegment(juce::Line<float>(rx - lineThickness / 2 + lineThickness / 100, centreY, rx + 15, centreY), lineThickness);
		p.applyTransform(juce::AffineTransform::rotation(tickAngle, centreX, centreY));
		g.fillPath(p);
		p.clear();
    }

	double lineThickness = 6;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnobLive)
};
