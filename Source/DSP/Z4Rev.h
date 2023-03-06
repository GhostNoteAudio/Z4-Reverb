#pragma once

#include "ModulatedDelay.h"
#include "ModulatedAllpass.h"
#include "Biquad.h"
#include "GranularPitchShift.h"
#include "../Parameters.h"

namespace Z4
{
    const int PRE_DIFFUSE_COUNT = 12;
    const int ZCOUNT = 4;

    class Z4Rev
    {
        GranularPitchShift<8000> pitchShifter1;
        GranularPitchShift<8000> pitchShifter2;
        ModulatedDelay<192000 / 8, BUFFER_SIZE> Delay[4]; // 125ms max delay, at 192Khz
        ModulatedAllpass<192000 / 10, BUFFER_SIZE> Diffuser[ZCOUNT * 2]; // 100ms max delay, at 192Khz
        ModulatedAllpass<192000/10, BUFFER_SIZE> PreDiffuser[PRE_DIFFUSE_COUNT]; // 100ms max delay, at 192Khz    
        Biquad lowPassPre, highPassPre;
        Biquad highShelfPost, lowShelfPost;
        Biquad highPassPost, lowPassPost;

        // Delay lengths in milliseconds, handpicked arbitrarily :)
        float PreDiffuserSizes[PRE_DIFFUSE_COUNT] = {56.797, 59.12, 65.1785, 67.324, 69.7954, 72.55, 75.6531, 80.804, 83.157, 86.45, 90.234, 96.194};
        float DiffuserSizes[ZCOUNT] = {70.312, 78.5123, 87.9312, 92.1576};
        float DelaySizes[ZCOUNT] = {73.459, 95.961, 104.1248, 117.934};

        // Modulation rates in Hz, I used sequential prime numbers scaled down
        float PreDiffuserModRate[PRE_DIFFUSE_COUNT] = {13*0.05, 17*0.05, 19*0.05, 23*0.05, 29*0.05};
        float DiffuserModRate[ZCOUNT] = {31*0.02, 37*0.02, 41*0.02, 43*0.02};
        float DelayModRate[ZCOUNT]    = {47*0.01, 53*0.01, 59*0.01, 61*0.01};

        int Samplerate;
        bool LeftOrRight;
        bool TrueStereo;
        float Krt;
        float T60;
        float Wet;
        float Dry;
        float EarlySize;
        float DiffuserSize;
        float LateSize;
        float Modulation;
        float ModRate;
        float DiffuseFeedback;
        int EarlyStages;
        bool freeze;
        float smoothedFreeze;
        int ShimmerMode;
        int SaturationMode;

    public:
        Z4Rev(int samplerate, bool leftOrRightChannel) : 
            lowPassPre(Biquad::FilterType::LowPass, samplerate),
            highPassPre(Biquad::FilterType::HighPass, samplerate),
            highShelfPost(Biquad::FilterType::HighShelf, samplerate),
            lowShelfPost(Biquad::FilterType::LowShelf, samplerate),
            lowPassPost(Biquad::FilterType::LowPass, samplerate),
            highPassPost(Biquad::FilterType::HighPass, samplerate),
            pitchShifter1(samplerate, 0.5),
            pitchShifter2(samplerate, 2.0)
        {
            Samplerate = samplerate;
            LeftOrRight = leftOrRightChannel;
            TrueStereo = false;
            Krt = 0.0;
            T60 = 5.0;
            Wet = 0.0;
            Dry = 1.0;
            EarlySize = 0.1;
            DiffuserSize = 1.0;
            LateSize = 0.1;
            Modulation = 0.2;
            ModRate = 0.5;
            DiffuseFeedback = 0.7;
            EarlyStages = 4;

            lowPassPre.Frequency = 20000;
            highPassPre.Frequency = 20;

            lowPassPost.Frequency = 14000;
            highPassPost.Frequency = 30;
                        
            lowShelfPost.SetGainDb(0);
            lowShelfPost.Frequency = 150;
            highShelfPost.SetGainDb(0);
            highShelfPost.Frequency = 4000;

            smoothedFreeze = 0;
            freeze = false;
            ShimmerMode = 0;

            UpdateAll();
        }

        bool IsTrueStereo()
        {
            return TrueStereo;
        }

        void ClearBuffers()
        {
            pitchShifter1.ClearBuffers();
            pitchShifter2.ClearBuffers();

            for (int i = 0; i < 4; i++)
                Delay[i].ClearBuffers();
            
            for (int i = 0; i < ZCOUNT * 2; i++)
                Diffuser[i].ClearBuffers();

            for (int i = 0; i < PRE_DIFFUSE_COUNT; i++)
                PreDiffuser[i].ClearBuffers();
            
            lowPassPre.ClearBuffers();
            highPassPre.ClearBuffers();
            highShelfPost.ClearBuffers();
            lowShelfPost.ClearBuffers();
            highPassPost.ClearBuffers();
            lowPassPost.ClearBuffers();
        }

        int GetSamplerate()
        {
            return Samplerate;
        }

        void SetSamplerate(int samplerate)
        {
            this->Samplerate = samplerate;
            lowPassPre.SetSamplerate(samplerate);
            highPassPre.SetSamplerate(samplerate);
            lowShelfPost.SetSamplerate(samplerate);
            highShelfPost.SetSamplerate(samplerate);
            lowPassPost.SetSamplerate(samplerate);
            highPassPost.SetSamplerate(samplerate);
            pitchShifter1.SetSamplerate(samplerate);
            pitchShifter2.SetSamplerate(samplerate);
            UpdateAll();
        }

        void SetParameter(int paramId, double value)
        {
            auto sv = ScaleParam(value, paramId);

            switch (paramId)
            {
            case Parameter::Dry:
                Dry = sv <= -30 ? 0 : Utils::DB2Gainf(sv);
                break;
            case Parameter::Wet:
                Wet = sv <= -30 ? 0 : Utils::DB2Gainf(sv);
                break;
            case Parameter::Decay:
                T60 = sv;
                break;
            case Parameter::Diffuse:
                DiffuseFeedback = 0.5 + (1 - sv) * 0.47;
                break;
            case Parameter::EarlySize:
                EarlySize = sv; // 10-100%
                break;
            case Parameter::LateSize:
                LateSize = sv; // 10-100%
                DiffuserSize = 0.2 + sv * 0.8;
                break;
            case Parameter::LowCut:
                highPassPre.Frequency = sv;
                break;
            case Parameter::HighCut:
                lowPassPre.Frequency = sv;
                break;
            case Parameter::Modulate:
                Modulation = sv;
                break;
            case Parameter::ModRate:
                ModRate = sv;
                break;
            case Parameter::Lows:
                lowShelfPost.SetGainDb(sv);
                break;
            case Parameter::Highs:
                highShelfPost.SetGainDb(sv);
                break;
            case Parameter::Stages:
                EarlyStages = (int)sv;
                break;
            case Parameter::Shimmer:
                ShimmerMode = (int)sv;
                break;
            case Parameter::Stereo:
                TrueStereo = sv == 1;
                ClearBuffers();
                break;
            case Parameter::Saturate:
                SaturationMode = (int)sv;
                break;
            case Parameter::Freeze:
                freeze = sv > 0.5;
                break;
            }

            UpdateAll();
        }

        void UpdateAll()
        {
            lowPassPre.Update();
            highPassPre.Update();
            lowShelfPost.Update();
            highShelfPost.Update();
            lowPassPost.Update();
            highPassPost.Update();

            const float IdealisedTimeConstant = 0.15 * std::sqrt(LateSize); // assumed tank round trip time
            auto tcToT60 = T60 / IdealisedTimeConstant;
            auto dbPerTc = -60 / tcToT60;
            Krt = std::pow(10, dbPerTc/20);

            for (size_t i = 0; i < PRE_DIFFUSE_COUNT; i++)
            {
                PreDiffuser[i].Feedback = 0.73;
                PreDiffuser[i].InterpolationEnabled = true;
                PreDiffuser[i].SampleDelay = (int)(PreDiffuserSizes[i] * 0.001 * EarlySize * Samplerate);
                PreDiffuser[i].ModRate = PreDiffuserModRate[i] / Samplerate * ModRate;
                PreDiffuser[i].ModAmount = Modulation * 60;
            }

            for (size_t i = 0; i < ZCOUNT; i++)
            {
                Diffuser[i].Feedback = DiffuseFeedback;
                Diffuser[i].InterpolationEnabled = true;
                Diffuser[i].SampleDelay = (int)(DiffuserSizes[i] * 0.001 * DiffuserSize * Samplerate);
                Diffuser[i].ModRate = DiffuserModRate[i] / Samplerate * ModRate;
                Diffuser[i].ModAmount = Modulation * 60;

                Delay[i].SampleDelay = (int)(DelaySizes[i] * 0.001 * LateSize * Samplerate);
                Delay[i].ModRate = DelayModRate[i] / Samplerate * ModRate;
                Delay[i].ModAmount = Modulation * (i == 0 ? 200 : 25); // extra mod on the first delay
            }
        }

        void Process(float* inL, float* inR, float* outL, float* outR, int bufSize)
        {
            float outLTemp[BUFFER_SIZE];
            float outRTemp[BUFFER_SIZE];

            while (bufSize > 0)
            {
                int subBufSize = bufSize > BUFFER_SIZE ? BUFFER_SIZE : bufSize;
                ProcessChunk(inL, inR, outLTemp, outRTemp, subBufSize);
                Utils::Copy(outL, outLTemp, bufSize);
                Utils::Copy(outR, outRTemp, bufSize);
                inL = &inL[subBufSize];
                inR = &inR[subBufSize];
                outL = &outL[subBufSize];
                outR = &outR[subBufSize];
                bufSize -= subBufSize;
            }
        }

    private:
        void ProcessChunk(float* inL, float* inR, float* outL, float* outR, int bufSize)
        {
            // Jumping straight between 100% feedback (freeze) and the selected feedback causes a click, needs to be smoothed
			smoothedFreeze = smoothedFreeze * 0.95 + (int)freeze * 0.05;
            float activeKrt = smoothedFreeze + (1-smoothedFreeze) * Krt;

            bool shimmerUp = (ShimmerMode == 1 || ShimmerMode == 3 || ShimmerMode == 5);
            bool shimmerDown = (ShimmerMode == 2 || ShimmerMode == 4 || ShimmerMode == 5);
            bool shimmerDirect = (ShimmerMode == 0 || ShimmerMode == 3 || ShimmerMode == 4 || ShimmerMode == 5);
            float shimmerGain = 1.0 / sqrtf((shimmerUp ? 1 : 0) + (shimmerDown ? 1 : 0) + (shimmerDirect ? 1 : 0));

            float buf[BUFFER_SIZE] = { 0 };
            float buf2[BUFFER_SIZE] = { 0 };
            float buf3[BUFFER_SIZE] = { 0 };
            bool useLeft = !LeftOrRight || !TrueStereo;
            bool useRight = LeftOrRight || !TrueStereo;
            float inGain = TrueStereo ? 1.0f : 0.5f;
            
            Utils::ZeroBuffer(buf, bufSize);
            if (useLeft)
                Utils::Mix(buf, inL, inGain, bufSize);
            if (useRight)
                Utils::Mix(buf, inR, inGain, bufSize);

            lowPassPre.Process(buf, buf, bufSize);
            highPassPre.Process(buf, buf, bufSize);

            float* preDiffIO = buf;
            for (size_t i = 0; i < PRE_DIFFUSE_COUNT; i++)
            {
                PreDiffuser[i].Process(preDiffIO, bufSize);
                preDiffIO = PreDiffuser[i].GetOutput();
            }

            preDiffIO = PreDiffuser[EarlyStages-1].GetOutput();

            for (size_t i = 0; i < ZCOUNT; i++)
            {
                Utils::Copy(buf, preDiffIO, bufSize);
                Utils::Mix(buf, Delay[(i - 1 + ZCOUNT) % ZCOUNT].GetOutput(), activeKrt, bufSize);
                if (i == 0)
                {
                    if (shimmerUp)
                        pitchShifter2.Process(buf, buf2, bufSize);
                    if (shimmerDown)
                        pitchShifter1.Process(buf, buf3, bufSize);

                    if (!shimmerDirect)
                        Utils::ZeroBuffer(buf, bufSize);
                    if (shimmerUp)
                        Utils::Mix(buf, buf2, 1.0f, bufSize);
                    if (shimmerDown)
                        Utils::Mix(buf, buf3, 1.0f, bufSize);
                    
                    Utils::Gain(buf, shimmerGain, bufSize);
                    if (SaturationMode == 1)
                        Utils::ApplyTanh(buf, bufSize);
                    else if (SaturationMode == 2)
                        Utils::Limit(buf, bufSize, -2.0f, 2.0f);

                    lowShelfPost.Process(buf, buf, bufSize);
                    highShelfPost.Process(buf, buf, bufSize);
                    highPassPost.Process(buf, buf, bufSize);
                    lowPassPost.Process(buf, buf, bufSize);
                }
                Diffuser[2*i].Process(buf, bufSize);
                Diffuser[2*i+1].Process(Diffuser[2*i].GetOutput(), bufSize);
                Delay[i].Process(Diffuser[2*i+1].GetOutput(), bufSize);
            }

            if (useLeft)
            {
                Utils::ZeroBuffer(outL, bufSize);
                Utils::Mix(outL, Delay[0].GetOutput(), Wet, bufSize);
                Utils::Mix(outL, Delay[2].GetOutput(), Wet, bufSize);
                Utils::Gain(outL, 0.5f, bufSize);
                Utils::Mix(outL, inL, Dry, bufSize);
            }

            if (useRight)
            {
                Utils::ZeroBuffer(outR, bufSize);
                Utils::Mix(outR, Delay[1].GetOutput(), Wet, bufSize);
                Utils::Mix(outR, Delay[3].GetOutput(), Wet, bufSize);
                Utils::Gain(outR, 0.5f, bufSize);
                Utils::Mix(outR, inR, Dry, bufSize);
            }

            
        }
        
    };
}
