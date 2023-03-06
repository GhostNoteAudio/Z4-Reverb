#pragma once

#include "Utils.h"
#include <stdint.h>

namespace Z4
{
	template<int DelayBufferSizeSamples, int BUF_SIZE>
	class ModulatedDelay
	{
	private:

		static const int ModulationUpdateRate = 8;

		float delayBuffer[DelayBufferSizeSamples];
		float output[BUF_SIZE];
		int writeIndex;
		int readIndexA;
		int readIndexB;
		uint64_t samplesProcessed;
		
		double modPhase;
		double gainA;
		double gainB;

	public:
		int SampleDelay;
		double ModAmount; // Sample excursion
		double ModRate; // Frequency / Samplerate

		ModulatedDelay()
		{
			SampleDelay = 1000;
			writeIndex = 0;
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

		void Process(float* input, int sampleCount)
		{
			for (int i = 0; i < sampleCount; i++)
			{
				if (samplesProcessed % ModulationUpdateRate == 0)
					Update();

				delayBuffer[writeIndex] = input[i];
				output[i] = delayBuffer[readIndexA] * gainA + delayBuffer[readIndexB] * gainB;

				writeIndex++;
				readIndexA++;
				readIndexB++;
				if (writeIndex >= DelayBufferSizeSamples) writeIndex -= DelayBufferSizeSamples;
				if (readIndexA >= DelayBufferSizeSamples) readIndexA -= DelayBufferSizeSamples;
				if (readIndexB >= DelayBufferSizeSamples) readIndexB -= DelayBufferSizeSamples;
				samplesProcessed++;
			}
		}

		void ClearBuffers()
		{
			Utils::ZeroBuffer(delayBuffer, DelayBufferSizeSamples);
			Utils::ZeroBuffer(output, BUF_SIZE);
		}

		void ResetPhase(float phase)
		{
			this->modPhase = fmodf(phase, 1.0f);
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

			auto delayA = (int)totalDelay;
			auto delayB = (int)totalDelay + 1;

			auto partial = totalDelay - delayA;

			gainA = 1 - partial;
			gainB = partial;

			readIndexA = writeIndex - delayA;
			readIndexB = writeIndex - delayB;
			if (readIndexA < 0) readIndexA += DelayBufferSizeSamples;
			if (readIndexB < 0) readIndexB += DelayBufferSizeSamples;
		}
	};
}
