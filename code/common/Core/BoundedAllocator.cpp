#include "BoundedAllocator.h"


BoundedAllocator::BoundedAllocator(const size_t aMaximumAllocationSize) noexcept
    : m_availableMemory{ aMaximumAllocationSize }
{
}

void* BoundedAllocator::Allocate(size_t aSize) noexcept
{
    if (m_availableMemory >= aSize)
    {
        m_availableMemory -= aSize;
        return MimallocAllocator::Allocate(aSize);
    }

    return nullptr;
}

void BoundedAllocator::Free(void* apData) noexcept
{
    m_availableMemory += Size(apData);

    MimallocAllocator::Free(apData);
}
