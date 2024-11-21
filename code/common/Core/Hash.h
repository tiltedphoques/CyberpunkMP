#pragma once

#include <cstdint>
#include <cstddef>

namespace FHash
{
    uint64_t Crc64(const unsigned char* acpData, std::size_t aLength);
    uint64_t FNV1a64(const char* aText, uint64_t aSeed = 0xCBF29CE484222325);

    template <class T> inline void Combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}
