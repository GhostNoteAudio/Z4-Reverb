#pragma once

#include <JuceHeader.h>

class RotaryKnobDot : public juce::Slider
{
public:
	inline RotaryKnobDot()
	{
		setRange(0.0, 1.0);
		setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 70, 25);
		setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
		
	}

	inline ~RotaryKnobDot() override
	{
	}

	inline void setSmall()
	{
		setSize(45, 45+25);
		lineThickness = 2.5;
	}

	inline void setMedium()
	{
		setSize(70,70+25);
	}

	inline void setLarge()
	{
		setSize(100, 100 + 25);
		lineThickness = 4;
	}

	inline void paint(juce::Graphics& g) override
	{
		auto sliderPos = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
		auto tbHeight = (getTextBoxPosition() >= TextBoxAbove) ? getTextBoxHeight() : 0.0;
		auto nominalWidth = 100.0;
		auto nomninalHeight = nominalWidth;
		auto scaleW = getWidth() / nominalWidth;
		auto scaleH = (getHeight() - tbHeight) / nomninalHeight;
		auto scale = juce::jmin(scaleW, scaleH);
		auto marginLeft = juce::jmax(getWidth() - scale * nominalWidth, 0.0) / 2;
		g.addTransform(juce::AffineTransform::scale(scale));
		g.addTransform(juce::AffineTransform::translation((float)marginLeft / scale, 0.0f));

		auto radius = nominalWidth / 2 - 4.0;
		auto centreX = nominalWidth * 0.5;
		auto centreY = nominalWidth * 0.5;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0;
		auto angle = (-45 + sliderPos * 270) / 360.0 * 2 * juce::MathConstants<double>::pi;

		auto colour = findColour(juce::Slider::thumbColourId);
		g.setColour(colour);
		g.drawEllipse(rx, ry, rw, rw, lineThickness);

		juce::Path p;

		auto dotSize = 2 + lineThickness + sqrt(lineThickness) * 2;
		p.addEllipse(6 + 1 * lineThickness, centreY - dotSize/2, dotSize, dotSize);
		p.applyTransform(juce::AffineTransform::rotation(angle, centreX, centreY));

		g.fillPath(p);
	}

	double lineThickness = 2.0;

};
