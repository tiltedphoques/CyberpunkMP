#pragma once

#include "Allocator.h"


struct ScratchAllocator : Allocator
{
    explicit ScratchAllocator(size_t aSize) noexcept;
    virtual ~ScratchAllocator();

    TP_NOCOPYMOVE(ScratchAllocator);

    [[nodiscard]] void* Allocate(size_t aSize) noexcept override;
    void Free(void* apData) noexcept override;
    [[nodiscard]] size_t Size(void* apData) noexcept override;
    void Reset() noexcept override;

private:

    size_t m_size;
    size_t m_baseSize;
    void* m_pData;
    void* m_pBaseData;
};

