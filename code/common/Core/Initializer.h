#pragma once

#include <cstdint>
#include <Core/Meta.h>


struct Initializer
{
    using TCallback = void (*)();

private:
    Initializer* m_pNext{ nullptr };
    TCallback m_callback{ nullptr };

    Initializer(Initializer*& parent, TCallback callback) noexcept;

public:
    Initializer(TCallback aCallback) noexcept;
    Initializer(Initializer& aParent, TCallback aCallback) noexcept;

    TP_NOCOPYMOVE(Initializer);

    static Initializer*& GetHead() noexcept;

    static size_t RunAll();
};

inline Initializer::Initializer(Initializer*& aParent, TCallback aCallback) noexcept
    : m_pNext(aParent)
    , m_callback(aCallback) 
{
    aParent = this;
}

inline Initializer::Initializer(TCallback aCallback) noexcept
    : Initializer(GetHead(), aCallback)
{}

inline Initializer::Initializer(Initializer& aParent, TCallback aCallback) noexcept
    : Initializer(aParent.m_pNext, aCallback)
{}

inline Initializer*& Initializer::GetHead() noexcept
{
    static Initializer* root{ nullptr };

    return root;
}

inline size_t Initializer::RunAll() 
{
    size_t total = 0;

    for (Initializer* i = GetHead(); i;)
    {
        if (i->m_callback)
        {
            i->m_callback();
            i->m_callback = nullptr;

            ++total;
        }

        Initializer* j = i->m_pNext;
        i->m_pNext = nullptr;
        i = j;
    }

    GetHead() = nullptr;

    return total;
}

