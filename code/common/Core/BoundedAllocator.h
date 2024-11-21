#pragma once

#include "MimallocAllocator.h"


struct BoundedAllocator : MimallocAllocator
{
    BoundedAllocator(size_t aMaximumAllocationSize) noexcept;
    virtual ~BoundedAllocator() = default;

    TP_NOCOPYMOVE(BoundedAllocator);

    [[nodiscard]] void* Allocate(size_t aSize) noexcept override;
    void Free(void* apData) noexcept override;

private:

    size_t m_availableMemory;
};

