#include "Allocator.h"
#include "MimallocAllocator.h"


static thread_local Allocator* s_pCurrentAllocator = nullptr;

void Allocator::Set(Allocator* apAllocator) noexcept
{
    s_pCurrentAllocator = apAllocator;
}

Allocator* Allocator::Get() noexcept
{
    if (s_pCurrentAllocator)
        return s_pCurrentAllocator;

    return GetDefault();
}

Allocator* Allocator::GetDefault() noexcept
{
    static MimallocAllocator s_allocator;
    return &s_allocator;
}

ScopedAllocator::ScopedAllocator(Allocator* apAllocator) noexcept
    : m_pOldAllocator(Allocator::Get())
{
    Allocator::Set(apAllocator);
}

ScopedAllocator::ScopedAllocator(Allocator& aAllocator) noexcept
    : ScopedAllocator(&aAllocator)
{
}

ScopedAllocator::~ScopedAllocator() noexcept
{
    Allocator::Set(m_pOldAllocator);
}

ScopedResetAllocator::ScopedResetAllocator(Allocator* apAllocator) noexcept
    : m_pOldAllocator(Allocator::Get())
{
    Allocator::Set(apAllocator);
}

ScopedResetAllocator::ScopedResetAllocator(Allocator& aAllocator) noexcept
    : ScopedResetAllocator(&aAllocator)
{
}

ScopedResetAllocator::~ScopedResetAllocator() noexcept
{
    Allocator::Get()->Reset();
    Allocator::Set(m_pOldAllocator);
}
