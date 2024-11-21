#pragma once

#include <Core/Hash.h>

template<class T>
struct Vector2 : glm::vec<2, T, glm::defaultp>
{
    Vector2(T x, T y)
    : glm::vec<2, T, glm::defaultp>(x, y)
    {
    }
};

namespace std
{
template <class T> struct hash<Vector2<T>>
{
    size_t operator()(const Vector2<T>& aVector) const noexcept
    {
        size_t hash = 0;
        FHash::Combine(hash, aVector.x);
        FHash::Combine(hash, aVector.y);
        return hash;
    }
};
} // namespace std