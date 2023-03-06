#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

namespace Z4
{
	class Lp1
	{
	private:
		float fs;
		float b0, a1;
		float cutoffHz;

	public:
		float Output;

		Lp1()
		{
			fs = 48000;
			b0 = 1;
			a1 = 0;
			cutoffHz = 1000;
		}

		float GetSamplerate()
		{
			return fs;
		}

		void SetSamplerate(float samplerate)
		{
			fs = samplerate;
		}

		float GetCutoffHz()
		{
			return cutoffHz;
		}

		void SetCutoffHz(float hz)
		{
			cutoffHz = hz;
			Update();
		}

		void ClearBuffers()
		{
			Output = 0;
		}

		void Update()
		{
			// Prevent going over the Nyquist frequency
			if (cutoffHz >= fs * 0.5f)
				cutoffHz = fs * 0.499f;

			auto x = 2.0f * M_PI * cutoffHz / fs;
			auto nn = (2.0f - cosf(x));
			auto alpha = nn - sqrtf(nn * nn - 1);

			a1 = alpha;
			b0 = 1 - alpha;
		}

		float Process(float input)
		{
			if (input == 0 && Output < 0.0000001f)
			{
				Output = 0;
			}
			else
			{
				Output = b0 * input + a1 * Output;
			}
			return Output;
		}

		void Process(float* input, float* output, int len)
		{
			for (int i = 0; i < len; i++)
				output[i] = Process(input[i]);
		}
	};
}
