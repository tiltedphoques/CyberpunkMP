#pragma once

#include <Windows.h>
#include "AutoPtr.h"

#include <mem/mem.h>
#include <mem/module.h>

#include "MemoryVP.h"


using TNameHandler = void(*)(void*, const char*);

namespace detail
{
    // dumbs down a class member ptr to a generic
    // void ptr, this is technically undefined behavior
    // but is verified to always work on MSVC
    // https://stackoverflow.com/questions/8121320/get-memory-address-of-member-function
    template<typename TOut, typename TIn>
    TOut PunType(TIn in)
    {
        union
        {
            TIn  in;
            TOut out;
        }
        u = { in };

        return u.out;
    };

    inline static mem::pointer currentBase;
    inline static TNameHandler nameHandler = nullptr;
}

class BaseOverrideContext final
{
public:
    inline BaseOverrideContext(const mem::pointer aNewPtr)
    {
        m_prevBase = detail::currentBase;
        detail::currentBase = aNewPtr;
    }

    inline ~BaseOverrideContext()
    {
        detail::currentBase = m_prevBase;
    }
private:
    mem::pointer m_prevBase;
};

inline void SetBase(const mem::pointer acBase, const mem::module* apOptModule)
{
    const mem::pointer disp = apOptModule ? acBase - apOptModule->start : acBase - 0x140000000;
    detail::currentBase = disp.as<ptrdiff_t>();
}

constexpr bool IsVAOnly(const mem::pointer aEa)
{
    // don't tune fixed addresses, which are bigger than a VA.
    return aEa <= UINT_MAX;
}

MEM_STRONG_INLINE void TuneBase(mem::pointer& arAddress)
{
    arAddress.add(detail::currentBase.as<size_t>());
}

template<typename T>
MEM_STRONG_INLINE void Put(mem::pointer aEa, const T acVal) noexcept
{
    if (IsVAOnly(aEa)) TuneBase(aEa);
    mem::protect({aEa, sizeof(T)}).copy(acVal);
}

MEM_STRONG_INLINE void Nop(mem::pointer aEa, size_t aLength) noexcept
{
    if (IsVAOnly(aEa)) TuneBase(aEa);
    const ScopedContext ctx(aEa, aLength);

    mem::region(aEa, aLength).fill(0x90);
}

// jump function
template<typename T>
MEM_STRONG_INLINE constexpr void Jump(mem::pointer aEa, const T& aFunc) noexcept
{
    if (IsVAOnly(aEa)) TuneBase(aEa);
    Put<uint8_t>(aEa, 0xE9);
    Put<int32_t>(aEa.as<intptr_t>() + 1, (intptr_t)aFunc - aEa.as<intptr_t>() - 5);
}

// jump class member function
template<typename T>
MEM_STRONG_INLINE constexpr void JumpCMF(mem::pointer aEa, const T& aFunc) noexcept
{
    if (IsVAOnly(aEa)) TuneBase(aEa);
    Jump(aEa, detail::PunType<const void*>(aFunc));
}

template <typename T>
MEM_STRONG_INLINE T GetCall(mem::pointer aEa)
{
    if (IsVAOnly(aEa)) TuneBase(aEa);

    // call length
    uintptr_t target = aEa.as<uintptr_t>() + 5;
    target += *reinterpret_cast<int32_t*>(aEa.as<uintptr_t>() + 1);

    return reinterpret_cast<T>(target);
}

template <typename TFunc>
MEM_STRONG_INLINE void PutCall(mem::pointer aEa, const TFunc& new_fn)
{
    if (IsVAOnly(aEa)) TuneBase(aEa);
    // put new call VA
    int32_t disp = static_cast<int32_t>(reinterpret_cast<uintptr_t>(*new_fn) - aEa.as<uintptr_t>() - 5);

    Put<uint8_t>(aEa, 0xE8);
    Put<int32_t>(aEa.as<uintptr_t>() + 1, disp);
}

template <typename TFunc>
MEM_STRONG_INLINE void SwapCall(mem::pointer aEa, TFunc& old_fn, const TFunc& new_fn)
{
    if (IsVAOnly(aEa)) TuneBase(aEa);

    old_fn = GetCall<TFunc>(aEa);
    // put new call VA
    int32_t disp = static_cast<int32_t>(reinterpret_cast<uintptr_t>(*new_fn) - aEa.as<uintptr_t>() - 5);

    Put<uint8_t>(aEa, 0xE8);
    Put<int32_t>(aEa.as<uintptr_t>() + 1, disp);
}

// Support code for reflection - don't use
MEM_STRONG_INLINE void SetNameHandler(TNameHandler aHandler) { detail::nameHandler = aHandler; }
MEM_STRONG_INLINE void InvokeNameHandler(const char* apcName) { detail::nameHandler(nullptr, apcName); }

