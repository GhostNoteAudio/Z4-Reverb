#pragma once

#include <JuceHeader.h>

class TrialMode
{
	int samplerate;
	int burstGapSecs;
	int burstLenSecs;
	int idx;
	float gate;
	bool enabled;
	juce::Random rnd;

public:
	inline TrialMode(bool enabled = true, int burstGapSecs = 10, int burstLenSecs = 1)
	{
		samplerate = 48000;
		gate = 0;
		this->burstGapSecs = burstGapSecs;
		this->burstLenSecs = burstLenSecs;
		idx = burstLenSecs * samplerate * 2;
		this->enabled = enabled;
	}

	inline void SetEnabled(bool enabled)
	{
		this->enabled = enabled;
	}

	inline void SetSamplerate(int samplerate)
	{
		this->samplerate = samplerate;
	}

	inline void Process(float* data, int bufSize, bool advanceTime = true)
	{
		if (!enabled)
			return;

		int totalLoopSamples = (burstGapSecs + burstLenSecs) * samplerate;
		int noiseActiveSamples = burstLenSecs * samplerate;

		for (int i = 0; i < bufSize; i++)
		{
			int g = ((idx + i) % totalLoopSamples) < noiseActiveSamples;
			gate = gate * 0.999 + g * 0.001;
			data[i] += (rnd.nextFloat() - 0.5f) * gate * 0.1;
		}
		
		if (advanceTime)
			idx += bufSize;
	}
};

