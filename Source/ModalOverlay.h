#pragma once

#include <JuceHeader.h>

class ModalOverlay  : public juce::Component
{
public:
	inline ModalOverlay()
    {
		
    }

	inline ~ModalOverlay() override
    {
    }

	inline void mouseDown(const juce::MouseEvent& ev) override
	{
		setVisible(false);
	}

	inline void paint (juce::Graphics& g) override
    {
		auto colour = juce::Colour::fromFloatRGBA(0, 0, 0, 0.7);
		this->toFront(true);
		setPaintingIsUnclipped(true);
		g.fillAll(colour);
    }

	inline void resized() override
    {
		if (component)
			component->setCentreRelative(0.5f, 0.5f);
    }

	inline void setComponent(Component* c)
	{
		if (this->component != nullptr)
		{
			removeAllChildren();
		}

		this->component = c;
		addAndMakeVisible(c);
		component->setCentreRelative(0.5f, 0.5f);
	}

	enum ColourIds
	{
		OverlayColour = 10000
	};

private:
	Component* component = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalOverlay)
};
