#pragma once

#include <JuceHeader.h>
#include "LayoutUtils.h"
#include "Authentication.h"
#include "ModalOverlay.h"

class LabelButton : public juce::Label
{
    juce::String text;
    bool isMouseOver;

public:
    inline LabelButton(juce::String text = "")
    {
        this->text = text;
        isMouseOver = false;
    }

    inline ~LabelButton() override
    {
    }

    inline void mouseEnter(const juce::MouseEvent& ev) override
    {
        isMouseOver = true;
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
        repaint();
    }

    inline void mouseExit(const juce::MouseEvent& ev) override
    {
        isMouseOver = false;
        setMouseCursor(juce::MouseCursor::NormalCursor);
        repaint();
    }

    inline void mouseDown(const juce::MouseEvent& ev) override
    {
        callback();
    }

    inline void paint(juce::Graphics& g) override
    {
        g.setFont(this->getFont());
        juce::Colour c = findColour(juce::Label::ColourIds::textColourId);
        c = c.withAlpha(isMouseOver ? 0.9f : 0.7f);
        g.setColour(c);
        g.drawText(text, 0, 0, getWidth(), getHeight(), juce::Justification::centred);
    }

    inline void setText(const juce::String& str)
    {
        this->text = str;
        repaint();
    }

    std::function<void()> callback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelButton)
};

class AboutDialog : public juce::Component
{
public:
    inline AboutDialog()
    {
        bool isFreeware = Authentication::GetIsFreeware();

        pluginName.setFont(getFontLight(24));
        pluginName.setText(juce::String(JucePlugin_Name) + " " + juce::String(JucePlugin_VersionString) + "\nDesigned by " + juce::String(JucePlugin_Manufacturer), 
            juce::NotificationType::sendNotificationAsync);
        pluginName.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
        pluginName.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(pluginName);

        webButton.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
        webButton.setText(juce::String(JucePlugin_ManufacturerWebsite));
        webButton.setFont(getFontLight(24));
        addAndMakeVisible(webButton);

        webButton.callback = []() {juce::URL(juce::String(JucePlugin_ManufacturerWebsite)).launchInDefaultBrowser(); };

        addAndMakeVisible(emailInput);
        emailInput.setEditable(!isFreeware);
        emailInput.setColour(juce::Label::backgroundColourId, juce::Colour::fromHSL(0, 0, 0.2, 1));
        emailInput.onTextChange = [this] {};

        addAndMakeVisible(licenseInput);
        licenseInput.setEditable(!isFreeware);
        licenseInput.setColour(juce::Label::backgroundColourId, juce::Colour::fromHSL(0, 0, 0.2, 1));
        licenseInput.onTextChange = [this] {};

        addAndMakeVisible(labelEmail);
        addAndMakeVisible(labelLicense);
        addAndMakeVisible(labelStatus);
        labelEmail.setText("Email:", juce::NotificationType::sendNotificationAsync);
        labelLicense.setText("License Key:", juce::NotificationType::sendNotificationAsync);
        labelEmail.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
        labelLicense.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
        labelStatus.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
        labelEmail.setFont(getFontRegular(16));
        labelLicense.setFont(getFontRegular(16));
        labelStatus.setFont(getFontRegular(20));

        auto isAuth = Authentication::IsAuthenticated();

        if (isAuth)
            labelStatus.setText("License is valid", juce::NotificationType::sendNotificationAsync);
        else
            labelStatus.setText("Plugin is in Demo Mode", juce::NotificationType::sendNotificationAsync);

        auto authData = Authentication::GetAuthData();
        if (authData.size() == 2)
        {
            auto emailData = authData[0];
            auto licenseData = isAuth ? "****-****-****-****" : authData[1];
            if (isFreeware)
            {
                licenseData = "Free Plugin";
                emailData = "";
            }

            emailInput.setText(emailData, juce::NotificationType::sendNotificationAsync);
            licenseInput.setText(licenseData, juce::NotificationType::sendNotificationAsync);
        }

        if (!isFreeware)
        {
            addAndMakeVisible(saveButton);
        }

        saveButton.setButtonText("Save");
        saveButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromHSL(0, 0, 0.2, 1));
        saveButton.onClick = [this, isAuth] {
            if (isAuth && (licenseInput.getText() == "****-****-****-****" || licenseInput.getText() == ""))
            {
                overlay->setVisible(false);
                return;
            }

            Authentication::StoreAuthData(emailInput.getText(), licenseInput.getText());
            overlay->setVisible(false);
            juce::AlertWindow::showMessageBoxAsync(
                juce::MessageBoxIconType::InfoIcon,
                "License Set",
                "Please restart the plugin for changes to take effect",
                "Ok",
                this
            );
        };
    }

    inline void SetOverlay(ModalOverlay* overlay)
    {
        this->overlay = overlay;
    }

    inline ~AboutDialog() override
    {
    }

    inline void resized() override
    {
        pluginName.setBounds(10, 30, getWidth() / 2 - 20, 50);
        webButton.setBounds(30, 100, getWidth() / 2 - 60, 24);

        emailInput.setBounds(getWidth() / 2 + 10, 50, getWidth() / 2 - 20, 24);
        licenseInput.setBounds(getWidth() / 2 + 10, 100, getWidth() / 2 - 20, 24);

        labelEmail.setBounds(getWidth() / 2 + 6, 30, 100, 20);
        labelLicense.setBounds(getWidth() / 2 + 6, 80, 100, 20);

        if (Authentication::GetIsFreeware())
        {
            labelStatus.setBounds(getWidth() / 2 + 10, 135, 250, 24);
        }
        else
        {
            saveButton.setBounds(getWidth() / 2 + 10, 135, 80, 24);
            labelStatus.setBounds(getWidth() / 2 + 100, 135, 250, 24);
        }
    }

    inline void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour::fromFloatRGBA(0.1, 0.1, 0.1, 1));
        g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 5);
        g.setColour(juce::Colour::fromHSL(0, 0, 1, 0.5));
        g.drawLine(getWidth() / 2, 10, getWidth() / 2, getHeight() - 10, 2.0);
    }

    ModalOverlay* overlay;

    juce::Label pluginName;
    LabelButton webButton;
    juce::Label emailInput;
    juce::Label licenseInput;
    juce::Label labelEmail;
    juce::Label labelLicense;
    juce::Label labelStatus;
    juce::TextButton saveButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutDialog)
};
