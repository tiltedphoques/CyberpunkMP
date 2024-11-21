#pragma once

#include "Allocator.h"

struct MimallocAllocator : Allocator
{
    MimallocAllocator() noexcept = default;
    virtual ~MimallocAllocator() = default;

    TP_NOCOPYMOVE(MimallocAllocator);

    [[nodiscard]] void* Allocate(size_t aSize) noexcept override;
    void Free(void* apData) noexcept override;
    [[nodiscard]] size_t Size(void* apData) noexcept override;

    [[nodiscard]] static void* AlignedAllocate(size_t aSize, size_t aAlignment) noexcept;
    static void AlignedFree(void* apData) noexcept;
};

