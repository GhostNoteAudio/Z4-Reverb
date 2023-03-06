#pragma once

#include <JuceHeader.h>
#include "Spinner.h"
#include "RoundToggle.h"

static juce::Colour ColourAlmostWhite = juce::Colour::fromRGB(252, 252, 252);
static juce::Colour ColourPrimary = juce::Colour::fromRGB(108, 188, 208);

static juce::Colour ColourTitle = juce::Colour::fromRGB(65, 114, 126);
static juce::Colour ColourLabels = juce::Colour::fromRGB(93, 158, 172);
static juce::Colour ColourSpinner = juce::Colour::fromRGB(40, 70, 77);

static juce::Colour ColourTrack = juce::Colour::fromFloatRGBA(0, 0, 0, 0.24);
static juce::Colour ColourKnobLabel = ColourPrimary.brighter(1);
static juce::Colour ColourMenuBackground = juce::Colour::fromRGB(240, 240, 240);
static juce::Colour ColourMenuText = juce::Colour::fromRGB(40, 40, 40);
static juce::Colour ColourMenuHighlight = juce::Colour::fromRGB(190, 190, 190);

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromHSL(0, 0, 0, 0.0));
        setColour(juce::Slider::trackColourId, ColourTrack);
        setColour(juce::Slider::thumbColourId, ColourPrimary);
        
        setDefaultSansSerifTypeface(getTypefaceRegular());

        setColour(Spinner::ColourIds::backgroundColourId, ColourSpinner);
        setColour(Spinner::ColourIds::textColourId, ColourPrimary);
        setColour(RoundToggleButton::ColourIds::fillColourId, ColourPrimary);
        setColour(juce::PopupMenu::ColourIds::backgroundColourId, ColourMenuBackground);
        setColour(juce::PopupMenu::ColourIds::textColourId, ColourMenuText);
        setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, ColourMenuHighlight);
        setColour(juce::TextButton::ColourIds::buttonColourId, ColourTitle);
        setColour(juce::TextButton::ColourIds::textColourOffId, ColourAlmostWhite);
        setColour(juce::TextEditor::ColourIds::outlineColourId, ColourTitle);
        setColour(juce::TextEditor::ColourIds::focusedOutlineColourId, ColourTitle);
        setColour(juce::TextEditor::ColourIds::textColourId, ColourMenuText);
        setColour(juce::TextEditor::ColourIds::backgroundColourId, ColourAlmostWhite);
    }

    void drawButtonBackground(juce::Graphics& g,
        juce::Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
        auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, 2);
    }
};

static CustomLookAndFeel CustomLaf;

inline void styleKnobLabel(juce::Label* label)
{
	label->setJustificationType(juce::Justification::centred);
	label->setColour(juce::Label::ColourIds::textColourId, ColourKnobLabel);
	label->setFont(getFontRegular(26));
}

inline void styleSpinnerLabel(juce::Label* label)
{
    label->setJustificationType(juce::Justification::centred);
    label->setColour(juce::Label::ColourIds::textColourId, ColourLabels);
    label->setFont(getFontRegular(32));
}

static juce::Image noiseImage(juce::Image::PixelFormat::ARGB, 2000, 1000, true);
static bool noiseDrawn = false;

inline void drawNoise(juce::Graphics& g, int x, int y, int w, int h)
{
    if (!noiseDrawn)
    {
        //juce::Image ig(juce::Image::PixelFormat::ARGB, w, h, true);
        juce::Random rand(0);
        for (int x = 0; x < 2000; x++)
        {
            for (int y = 0; y < 1000; y++)
            {
                float colour = rand.nextFloat() < 0.2 ? 0 : 1;
                float alpha = colour == 0 ? 0.05 : 0.02;
                noiseImage.setPixelAt(x, y, juce::Colour::fromFloatRGBA(colour, colour, colour, rand.nextFloat() * alpha));
            }
        }
        noiseDrawn = true;
    }

    g.setColour(juce::Colours::white); // why TF do I need to set this?!
    g.drawImage(noiseImage, x, y, w, h, 0, 0, w, h, false);
}
