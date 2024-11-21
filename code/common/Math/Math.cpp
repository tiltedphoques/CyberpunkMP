#include "Math.h"
#include <cmath>


float Sigmoid(float x, float lambda, float k) noexcept
{
    return 1.f / (1.f + expf(-x * lambda + k));
}

float SmoothStep(float delta) noexcept
{
    return delta * delta * (3 - 2 * delta);
}

float DeltaAngle(float from, float to, bool useRadians) noexcept
{
    const float halfCircle = useRadians ? float(Pi) : 180.f;

    if (fabs(from - to) >= halfCircle)
    {
        if (from < to)
        {
            return to - (from + halfCircle * 2);
        }

        return (to + halfCircle * 2) - from;
    }

    return to - from;
}

float Mod(float aValue, float aMax) noexcept
{
    return fmod(aValue, aMax);
}

float Max(float aVal, float aMax) noexcept
{
    return aVal < aMax ? aMax : aVal;
}

float Min(float aVal, float aMin) noexcept
{
    return aVal > aMin ? aMin : aVal;
}

float Clamp(float aValue, float aMin, float aMax) noexcept
{
    return Max(aMin, Min(aValue, aMax));
}

float Sqrt(float aValue) noexcept
{
    return sqrt(aValue);
}

double Sqrt(double aValue) noexcept
{
    return sqrt(aValue);
}

