#pragma once

#define _USE_MATH_DEFINES 1
#include <math.h>
#include <stdint.h>

namespace Utils
{
    inline float tanhm(const float x, const float m)
    {
        // m = 0...1, controls "hardness"
        const float x2 = x * x;
        const float x3 = x2 * x;
        const float x4 = x3 * x;
        const float x5 = x4 * x;
        const float m2 = m * m;
        const float ax = fabsf(x);

        return((x * m + 0.46247385533802f * x * ax + x3 * ax + x5 * m2) /
            (m + x4 + 0.423947629220444f * ax + 0.46247385533802f * x2 +
                x4 * m2 * ax));
    }

    inline void ApplyTanh(float* data, int bufSize)
    {
        for (int i = 0; i < bufSize; i++)
        {
            data[i] = tanhm(data[i], 0.5);
        }
    }

    inline float Limit(float val, float min, float max)
    {
        return val < min ? min : val > max ? max : val;
    }

    inline void Limit(float* data, int bufSize, float min, float max)
    {
        for (int i = 0; i < bufSize; i++)
        {
            auto val = data[i];
            data[i] = val < min ? min : val > max ? max : val;
        }
    }

    inline float Clamp(float val)
    {
        return val < 0 ? 0 : val > 1 ? 1 : val;
    }

    inline uint16_t To12Bit(float value)
    {
        return 2048 + value * 2047;
    }

    template<typename T>
    inline T Clip1(T value)
    {
        return value > 1 ? 1 : (value < -1 ? -1 : value);
    }

    template<typename T>
    inline T ClipF(T value, T min, T max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    template<typename T>
    inline void Clip1(T* value, int len)
    {
        for (int i = 0; i < len; i++)
        {
            T v = value[i];
            value[i] = v > 1 ? 1 : (v < -1 ? -1 : v);
        }
    }

    inline int ClipI(int value, int min, int max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    template<typename T>
    inline void ZeroBuffer(T* buffer, int len)
    {
        for (int i = 0; i < len; i++)
            buffer[i] = 0;
    }

    template<typename T>
    inline void Copy(T* dest, T* source, int len)
    {
        memcpy(dest, source, len * sizeof(T));
    }

    template<typename T>
    inline void Gain(T* buffer, T gain, int len)
    {
        for (int i = 0; i < len; i++)
        {
            buffer[i] *= gain;
        }
    }

    template<typename T, typename K>
    inline void Multiply(T* dest, K* modulator, int len)
    {
        for (int i = 0; i < len; i++)
        {
            dest[i] *= modulator[i];
        }
    }

    template<typename T, typename K>
    inline void Multiply1(T* dest, K* modulator, int len)
    {
        for (int i = 0; i < len; i++)
        {
            dest[i] *= (1.0f + modulator[i]);
        }
    }

    template<typename T>
    inline void Mix(T* target, T* source, T gain, int len)
    {
        for (int i = 0; i < len; i++)
            target[i] += source[i] * gain;
    }

    inline float MaxAbsF(float* data, int len)
    {
        float max = -99999999;
        for (int i = 0; i < len; i++)
        {
            float val = fabsf(data[i]);
            max = val > max ? val : max;
        }
        return max;
    }

    template<typename T>
    inline T Max(T* data, int len)
    {
        T max = std::numeric_limits<T>::min();
        for (int i = 0; i < len; i++)
        {
            T val = data[i];
            max = val > max ? val : max;
        }
        return max;
    }

    template<typename T>
    inline T Min(T* data, int len)
    {
        T min = std::numeric_limits<T>::max();
        for (int i = 0; i < len; i++)
        {
            T val = data[i];
            min = val < min ? val : min;
        }
        return min;
    }

    inline float Max(float* data, int len)
    {
        float max = -999999999999;
        for (int i = 0; i < len; i++)
        {
            float val = data[i];
            max = val > max ? val : max;
        }
        return max;
    }

    inline float Min(float* data, int len)
    {
        float min = 999999999999;
        for (int i = 0; i < len; i++)
        {
            float val = data[i];
            min = val < min ? val : min;
        }
        return min;
    }

    inline double Max(double* data, int len)
    {
        double max = -999999999999;
        for (int i = 0; i < len; i++)
        {
            double val = data[i];
            max = val > max ? val : max;
        }
        return max;
    }

    inline double Min(double* data, int len)
    {
        double min = 999999999999;
        for (int i = 0; i < len; i++)
        {
            double val = data[i];
            min = val < min ? val : min;
        }
        return min;
    }

    template<typename T>
    inline T Mean(T* data, int len)
    {
        double sum = 0;
        for (int i = 0; i < len; i++)
        {
            sum += data[i];
        }
        return (T)(sum / len);
    }

    inline float DB2Gainf(float input)
    {
        //return std::pow(10.0f, input / 20.0f);
        return powf(10, input * 0.05f);
    }

    template<typename T>
    inline double DB2Gain(T input)
    {
        return pow10f(input / 20.0);
    }

    template<typename T>
    inline double Gain2DB(T input)
    {
        //if (input < 0.0000001)
        //    return -100000;

        return 20.0f * log10f(input);
    }

    const float dec1Mult = (10 / 9.0) * 0.1;
    const float dec2Mult = (100 / 99.0) * 0.01;
    const float dec3Mult = (1000 / 999.0) * 0.001;
    const float dec4Mult = (10000 / 9999.0) * 0.0001;

    const float oct1Mult = (2 / 1.0) * 0.5;
    const float oct2Mult = (4 / 3.0) * 0.25;
    const float oct3Mult = (8 / 7.0) * 0.125;
    const float oct4Mult = (16 / 15.0) * 0.0625;
    const float oct5Mult = (32 / 31.0) * 0.03125;
    const float oct6Mult = (64 / 63.0) * 0.015625;
    const float oct7Mult = (128 / 127.0) * 0.0078125;
    const float oct8Mult = (256 / 255.0) * 0.00390625;

    inline float Resp1dec(float x) { return (powf(10, x) - 1) * dec1Mult; }
    inline float Resp2dec(float x) { return (powf(10, 2 * x) - 1) * dec2Mult; }
    inline float Resp3dec(float x) { return (powf(10, 3 * x) - 1) * dec3Mult; }
    inline float Resp4dec(float x) { return (powf(10, 4 * x) - 1) * dec4Mult; }

    inline float Resp1oct(float x) { return (powf(2, x) - 1) * oct1Mult; }
    inline float Resp2oct(float x) { return (powf(2, 2 * x) - 1) * oct2Mult; }
    inline float Resp3oct(float x) { return (powf(2, 3 * x) - 1) * oct3Mult; }
    inline float Resp4oct(float x) { return (powf(2, 4 * x) - 1) * oct4Mult; }
    inline float Resp5oct(float x) { return (powf(2, 5 * x) - 1) * oct5Mult; }
    inline float Resp6oct(float x) { return (powf(2, 6 * x) - 1) * oct6Mult; }
    inline float Resp7oct(float x) { return (powf(2, 7 * x) - 1) * oct7Mult; }
    inline float Resp8oct(float x) { return (powf(2, 8 * x) - 1) * oct8Mult; }

    // Truncates the end of an IR using a cosine window
    inline void TruncateCos(float* data, int dataSize, float fraction)
    {
        int sample_count = (int)(fraction * dataSize);
        int offset = dataSize - sample_count;
        for (int i = 0; i < dataSize; i++)
        {
            data[offset + i] *= cosf(i / (float)sample_count * M_PI * 0.5);
        }
    }

    inline void ApplyHamming(float* buffer, int size)
    {
        int M = size - 1;

        for (int n = 0; n < size; n++)
        {
            float val = 0.54 - 0.46 * cosf(2 * M_PI * n / (double)M) + 0.08 * cosf(4 * M_PI * n / (double)M);
            buffer[n] *= val;
        }
    }

    inline float Sinc(float x)
    {
        return x == 0 ? 1 : sinf(M_PI * x) / (M_PI * x);
    }

    // Note: choose N as an odd number
    inline void MakeSincFilter(float* buffer, int N, float fmin, float fmax, int Fs)
    {
        fmin = fmin / (Fs * 0.5);
        fmax = fmax / (Fs * 0.5);

        for (int i = 0; i < N; i++)
        {
            int x = i - N / 2;
            float val = fmax * Sinc(x * fmax) - fmin * Sinc(x * fmin);
            buffer[i] = val;
        }

        ApplyHamming(buffer, N);
    }

    inline float Sum(float* data, int count)
    {
        float output = 0.0f;
        for (int i = 0; i < count; i++)
        {
            output += data[i];
        }
        return output;
    }

    inline float Note2hz(float note)
    {
        const float scaler = 1 / 12.0;
        //return powf(2, (note-69)/12.0f) * 440.0f;
        return powf(2, (note - 69) * scaler) * 440.0f;
    }
}

