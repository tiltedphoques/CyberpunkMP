#pragma once

#include <Core/Stl.h>


struct Pattern
{
    enum EType
    {
        kRelativeIndirection4,
        kDirect
    };

    Pattern(Vector<uint8_t> aBytePattern, size_t aExpected, EType aPatternType, intptr_t aOffset = 0, size_t aIndex = 0) noexcept;

    Vector<uint8_t> BytePattern;
    size_t Expected;
    EType Type;
    intptr_t Offset{ 0 };
    size_t Index{ 0 };
};


namespace std
{
    template<> struct less<Pattern>
    {
        bool operator() (const Pattern& lhs, const Pattern& rhs) const noexcept
        {
            return lhs.BytePattern < rhs.BytePattern;
        }
    };
}
