#pragma once

#include <JuceHeader.h>
#include <vector>
#include "Stylers.h"

namespace Presets
{
    void deleteCurrentPreset(Z4ReverbAudioProcessor* processor);
    void loadPreset(Z4ReverbAudioProcessor* processor, juce::String presetName);
    void savePreset(Z4ReverbAudioProcessor* processor, juce::String presetName);
    std::vector<juce::String> getPresets();
    juce::String getPluginResourceDir();
    juce::String getPresetDir();

    inline void showPresetMenu(
        Z4ReverbAudioProcessor* processor,
        std::function<void()> onLoad,
        std::function<void()> onSave)
    {
        auto presets = getPresets();
        juce::PopupMenu m;
        m.setLookAndFeel(&CustomLaf);
        m.addItem(1, "Save Preset");
        m.addItem(2, "Delete Preset");
        m.addSeparator();

        for (int i = 0; i < presets.size(); i++)
            m.addItem(10000 + i, presets[i]);

        m.showMenuAsync(juce::PopupMenu::Options(),
            [onLoad, presets, onSave, processor](int result)
            {
                if (result == 0)
                    return;

                if (result == 1)
                {
                    onSave();
                }
                else if (result == 2)
                {
                    deleteCurrentPreset(processor);
                    onLoad();
                }
                else if (result >= 10000)
                {
                    loadPreset(processor, presets[result - 10000]);
                    onLoad();
                }
            });
    }

    inline void loadPreset(Z4ReverbAudioProcessor* processor, juce::String presetName)
    {
        auto filename = getPresetDir() + juce::File::getSeparatorChar() + presetName + ".xml";
        juce::File file(filename);
        juce::MemoryBlock block;
        uint32_t xmlMagic = 0x21324356;
        uint32_t zero = 0;

        if (file.existsAsFile())
        {
            block.append(&xmlMagic, 4);
            uint32_t size = file.getSize();
            block.append(&size, 4);
            file.loadFileAsData(block);
            block.append(&zero, 1);

            processor->setStateInformation(block.begin(), block.getSize());
        }
    }

    inline void deleteCurrentPreset(Z4ReverbAudioProcessor* processor)
    {
        auto presetName = processor->getPresetName();
        auto filename = getPresetDir() + juce::File::getSeparatorChar() + presetName + ".xml";
        juce::File file(filename);
        if (file.existsAsFile())
        {
            file.deleteFile();
            loadPreset(processor, getPresets()[0]);
        }
    }

    inline void savePreset(Z4ReverbAudioProcessor* processor, juce::String presetName)
    {
        juce::MemoryBlock block;
        processor->getStateInformation(block);
        auto size = block.getSize();
        auto filename = getPresetDir() + juce::File::getSeparatorChar() + presetName + ".xml";
        juce::File file(filename);
        file.create();
        file.replaceWithData(&block[8], block.getSize() - 9);
        processor->setPresetName(presetName);
    }

    inline std::vector<juce::String> getPresets()
    {
        auto presetDir = getPresetDir();
        auto files = juce::File(presetDir).findChildFiles(juce::File::findFiles, false);
        std::vector<juce::String> output;
        for (auto& it : files)
        {
            output.push_back(it.getFileNameWithoutExtension());
        }
        return output;
    }

    inline juce::String getPluginResourceDir()
    {
        auto appData = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory);
        auto path = appData.getFullPathName()
            + juce::File::getSeparatorChar() + juce::String(JucePlugin_Manufacturer)
            + juce::File::getSeparatorChar() + juce::String(JucePlugin_Desc);
        return path;
    }

    inline juce::String getPresetDir()
    {
        auto path = juce::String(getPluginResourceDir()) + juce::File::getSeparatorChar() + "Presets";
        return path;
    }
}
