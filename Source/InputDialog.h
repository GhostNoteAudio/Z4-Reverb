#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <functional>
#include "RoundedPanel.h"
#include "Stylers.h"

class InputDialog  : public RoundedPanel
{
public:
    inline InputDialog()
    {
		setSize(450, 200);
    }

	inline void setContent(std::string caption, std::vector<std::string> buttonText, std::function<void(std::string, int)> onClick)
	{
		this->caption = caption;
		this->onClick = onClick;

		for (size_t i = 0; i < buttons.size(); i++)
		{
			delete buttons[i];
		}

		buttons.clear();

		for (size_t i = 0; i < buttonText.size(); i++)
		{
			auto btnText = buttonText[i];
			auto btn = new juce::TextButton(btnText);
			
			btn->onClick = [this, onClick, i] { onClick(this->getValue(), i); };
			this->buttons.push_back(btn);
			addAndMakeVisible(btn);
		}

		textEditor.clear();
		addAndMakeVisible(textEditor);
		textEditor.setMultiLine(false);
		resized();
	}

	inline void paint(juce::Graphics& g) override
	{
		RoundedPanel::paint(g);
		g.setColour(ColourTitle);
		g.setFont(getFontLight(24));
		g.drawFittedText(caption, 10, 10, getWidth() - 20, 50, juce::Justification::centredTop, 3);
	}

	inline ~InputDialog() override
    {
		for (size_t i = 0; i < buttons.size(); i++)
		{
			delete buttons[i];
		}
    }

	inline void resized() override
    {
		textEditor.setFont(getFontRegular(16));
		textEditor.setBounds(getWidth() / 2 - 200, 70, 400, 25);
		auto buttonWidth = 100;
		auto buttonMargin = 15;
		auto totalWidth = buttonWidth * buttons.size() + buttonMargin * (buttons.size() - 1);
		auto xMargin = (getWidth() - totalWidth) / 2.0;
		for (size_t i = 0; i < buttons.size(); i++)
		{
			buttons[i]->setBounds(xMargin + i * (buttonWidth + buttonMargin), getHeight() - 25 - 15, buttonWidth, 25);
		}
    }

	inline std::string getValue()
	{
		return this->textEditor.getTextValue().toString().toStdString();
	}

private:
	std::string caption;
	std::function<void(std::string, int)> onClick;
	juce::TextEditor textEditor;
	std::vector<juce::TextButton*> buttons;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputDialog)
};
