#pragma once

#include <JuceHeader.h>
#include "DSP/Utils.h"

namespace Parameter
{
    const int Dry = 0;
    const int Wet = 1;
    const int Decay = 2;
    const int Diffuse = 3;
    const int EarlySize = 4;
    const int LateSize = 5;
    const int LowCut = 6;
    const int HighCut = 7;
    const int Modulate = 8;
    const int ModRate = 9;
    const int Lows = 10;
    const int Highs = 11;

    const int Stages = 12;
    const int Shimmer = 13;
    const int Stereo = 14;
    const int Saturate = 15;
    const int Freeze = 16;

    const int COUNT = 17;
};

extern const char* ParameterIds[Parameter::COUNT];
extern const char* ParameterLabel[Parameter::COUNT];

inline double ScaleParam(double val, int index)
{
    switch (index)
    {
    case Parameter::Dry:
    case Parameter::Wet:
        return -30 + val * 30;

    case Parameter::Decay:
        return 0.05 + Utils::Resp3dec(val) * 59.95;
    case Parameter::Diffuse:
        return val;

    case Parameter::EarlySize:
    case Parameter::LateSize:
        return 0.1 + val * 0.9;

    case Parameter::LowCut:
        return 20 + Utils::Resp4oct(val) * 1980;
    case Parameter::HighCut:
        return 200 + Utils::Resp4oct(val) * 15800;

    case Parameter::Modulate:
        return val;
    case Parameter::ModRate:
        return val * 2;
        
    case Parameter::Lows:
    case Parameter::Highs:
        return -10 + val * 10;

    case Parameter::Stages:
        return (int)(1 + val * 11.999);
    case Parameter::Shimmer:
        return (int)(val * 5.999);
    case Parameter::Stereo:
        return val > 0.5 ? 1 : 0;
    case Parameter::Saturate:
        return (int)(val * 2.999);
    case Parameter::Freeze:
        return val > 0.5 ? 1 : 0;
    }
    return 0;
}

inline juce::String FormatParameter(float val, int maxLen, int paramId)
{
    double s = ScaleParam(val, paramId);

    switch (paramId)
    {
    case Parameter::Dry:
    case Parameter::Wet:
        return s <= -30 ? "Muted" : juce::String(s, 1) + " dB";

    case Parameter::Decay:
        if (s < 1)
            return juce::String((int)(s * 1000)) + " ms";
        else if (s < 10)
            return juce::String(s, 2) + " sec";
        else
            return juce::String(s, 1) + " sec";

    case Parameter::Diffuse:
    case Parameter::EarlySize:
    case Parameter::LateSize:
        return juce::String((int)(val * 100)) + "%";

    case Parameter::LowCut:
    case Parameter::HighCut:
        return juce::String((int)s) + " Hz";
    
    case Parameter::Modulate:
        return juce::String((int)(s * 100)) + "%";

    case Parameter::ModRate:
        return juce::String((int)(s*100)) + "%"; // yes this is correct...

    case Parameter::Lows:
    case Parameter::Highs:
        return juce::String(s, 1) + " dB";

    case Parameter::Stages:
        return juce::String((int)s);

    case Parameter::Shimmer:
        if (s == 0)
            return juce::String("Off");
        else if (s == 1)
            return juce::String("Up");
        else if (s == 2)
            return juce::String("Down");
        else if (s == 3)
            return juce::String("+ Up");
        else if (s == 4)
            return juce::String("+ Down");
        else if (s == 5)
            return juce::String("+ U/D");

    case Parameter::Stereo:
    case Parameter::Freeze:
        return s == 1 ? "Enabled" : "Disabled";

    case Parameter::Saturate:
        if (s == 0)
            return "Off";
        else if (s == 1)
            return "Soft";
        else if (s == 2)
            return "Hard";

    default:
        return juce::String(s, 2);
    }
}
