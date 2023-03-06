#pragma once

#include "Utils.h"
#include <stdint.h>

namespace Z4
{
	template<int DelayBufferSamples, int BUF_SIZE>
	class ModulatedAllpass
	{
	public:
		static const int ModulationUpdateRate = 8;

	private:
		float delayBuffer[DelayBufferSamples];
		float output[BUF_SIZE];
		float fsInv;
		int index;
		unsigned int samplesProcessed;

		float modPhase;
		int delayA;
		int delayB;
		float gainA;
		float gainB;

	public:
		int SampleDelay;
		float Feedback;
		float ModAmount; // Sample excursion
		float ModRate; // Frequency / Samplerate
		bool InterpolationEnabled;

		ModulatedAllpass()
		{
			this->InterpolationEnabled = true;
			SampleDelay = 100;
			Feedback = 0;
			index = DelayBufferSamples - 1;
			modPhase = 0.0;
			ModRate = 0.0;
			ModAmount = 0.0;
			ClearBuffers();
			Update();
		}

		float* GetOutput()
		{
			return output;
		}

		void ClearBuffers()
		{
			Utils::ZeroBuffer(delayBuffer, DelayBufferSamples);
			Utils::ZeroBuffer(output, BUF_SIZE);
		}

		void ResetPhase(float phase)
		{
			this->modPhase = fmodf(phase, 1.0f);
		}

		void Process(float* input, int sampleCount)
		{
			bool interp = InterpolationEnabled;

			for (int i = 0; i < sampleCount; i++)
			{
				if (samplesProcessed >= ModulationUpdateRate)
					Update();

				float bufOut;

				if (interp)
				{
					int idxA = index - delayA;
					int idxB = index - delayB;
					idxA += DelayBufferSamples * (idxA < 0); // modulo
					idxB += DelayBufferSamples * (idxB < 0); // modulo

					bufOut = delayBuffer[idxA] * gainA + delayBuffer[idxB] * gainB;
				}
				else
				{
					int idxA = index - delayA;
					idxA += DelayBufferSamples * (idxA < 0); // modulo
					bufOut = delayBuffer[idxA];
				}

				auto inVal = input[i] + bufOut * Feedback;
				delayBuffer[index] = inVal;
				output[i] = bufOut - inVal * Feedback;

				index++;
				if (index >= DelayBufferSamples) index -= DelayBufferSamples;

				samplesProcessed++;
			}
		}

	private:
		void Update()
		{
			modPhase += ModRate * ModulationUpdateRate;
			if (modPhase > 1)
				modPhase -= 1.0;

			auto mod = sinf(modPhase * 2 * M_PI);

			if (ModAmount >= SampleDelay) // don't modulate to negative value
				ModAmount = SampleDelay - 1;

			auto totalDelay = SampleDelay + ModAmount * mod;
			
			delayA = (int)totalDelay;
			delayB = (int)totalDelay + 1;

			auto partial = totalDelay - delayA;

			gainA = 1 - partial;
			gainB = partial;

			samplesProcessed = 0;
		}
	};
}
