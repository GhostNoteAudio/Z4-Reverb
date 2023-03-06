#pragma once

#include <JuceHeader.h>

inline void fitLabelSize(juce::Label* l, int errorPadding = 0)
{
	auto split_string_by_newline = [](const std::string& str)
	{
		auto result = std::vector<std::string>{};
		auto ss = std::stringstream{ str };

		for (std::string line; std::getline(ss, line, '\n');)
			result.push_back(line);

		return result;
	};

	auto stringLines = split_string_by_newline(l->getText().toStdString());
	auto maxWidth = 0.0;
	for (auto ss : stringLines)
	{
		auto w = l->getFont().getStringWidthFloat(ss);
		if (w > maxWidth)
			maxWidth = w;
	}
	auto h = l->getFont().getHeight() * stringLines.size();
	l->setSize(maxWidth + errorPadding, h);
}

template <typename T>
inline void hCenter(juce::Component* c, float y, juce::Rectangle<T> rect)
{
	c->setTopLeftPosition((rect.getWidth() - (double)c->getWidth()) / 2.0, y);
}

template <typename T>
inline juce::Rectangle<T> getCenterRect(juce::Rectangle<T>& rect, int width, int height, int y)
{
	return juce::Rectangle<T>((rect.getWidth() - width) / 2, y, width, height);
}


inline void attachLabelToComponent(juce::Label& label, juce::Component& c, std::string text)
{
	label.setText(text, juce::NotificationType::sendNotification);
	label.attachToComponent(&c, false);
	label.setJustificationType(juce::Justification::centred);
}

static const juce::ReferenceCountedObjectPtr<juce::Typeface> getTypefaceSemibold()
{
	return juce::Typeface::createSystemTypefaceFor(BinaryData::OpenSansSemiBold_ttf, BinaryData::OpenSansSemiBold_ttfSize);
}

static const juce::ReferenceCountedObjectPtr<juce::Typeface> getTypefaceRegular()
{
	return juce::Typeface::createSystemTypefaceFor(BinaryData::OpenSansRegular_ttf, BinaryData::OpenSansRegular_ttfSize);
}

static const juce::ReferenceCountedObjectPtr<juce::Typeface> getTypefaceLight()
{
	return juce::Typeface::createSystemTypefaceFor(BinaryData::OpenSansLight_ttf, BinaryData::OpenSansLight_ttfSize);
}

static inline juce::Font getFontSemibold(float size)
{
	auto ff = juce::Font(getTypefaceSemibold());
	ff.setHeight(size);
	return ff;
}

static inline juce::Font getFontRegular(float size)
{
	auto ff = juce::Font(getTypefaceRegular());
	ff.setHeight(size);
	return ff;
}

static inline juce::Font getFontLight(float size)
{
	auto ff = juce::Font(getTypefaceLight());
	ff.setHeight(size);
	return ff;
}
