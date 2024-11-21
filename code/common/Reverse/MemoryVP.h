#pragma once

#include <Core/Platform.h>

#include <mem/mem.h>
#include <mem/protect.h>

class ScopedContext
{
public:
    inline ScopedContext(const mem::pointer aEa, size_t aSize) noexcept;
    inline ~ScopedContext();

    template<class T>
    bool Write(const T& acData, const size_t aOffset = 0) const noexcept
    {
        return WriteBuffer(reinterpret_cast<const unsigned char*>(&acData), sizeof(T), aOffset);
    }

    inline bool WriteBuffer(const uint8_t* acpData, size_t aSize, size_t aOffset) const noexcept;

private:
    size_t m_size;
    uint32_t m_oldProtect;
    mem::pointer m_ea;
};


#if TP_PLATFORM_WINDOWS
#include "Memory_Win.inl"
#endif
// TODO: maybe Linux?
