#pragma once


constexpr double Pi = 3.14159265358979323846;

[[nodiscard]] float Sigmoid(float x, float lambda, float k) noexcept;
[[nodiscard]] float SmoothStep(float delta) noexcept;
[[nodiscard]] float DeltaAngle(float from, float to, bool useRadians) noexcept;
[[nodiscard]] float Mod(float aValue, float aMax) noexcept;
[[nodiscard]] float Max(float aVal, float aMax) noexcept;
[[nodiscard]] float Min(float aVal, float aMin) noexcept;
[[nodiscard]] float Clamp(float aValue, float aMin, float aMax) noexcept;
[[nodiscard]] float Sqrt(float aValue) noexcept;
[[nodiscard]] double Sqrt(double aValue) noexcept;

template <class T> [[nodiscard]]
T LerpDelta(const T& a, const T& b, float delta) noexcept
{
    return (b - a) * delta;
}

// delta should be between 0.f & 1.f
template <class T> [[nodiscard]]
T Lerp(const T& a, const T& b, float delta) noexcept
{
    return a + LerpDelta(a, b, delta);
}
