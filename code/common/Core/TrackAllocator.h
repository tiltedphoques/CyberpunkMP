#pragma once

#include "Allocator.hpp"


{
    template<class T>
    struct TrackAllocator : Allocator
    {
        TrackAllocator() noexcept = default;
        virtual ~TrackAllocator() {}

        TP_NOCOPYMOVE(TrackAllocator);

        [[nodiscard]] void* Allocate(size_t aSize) noexcept override
        {
            void* pData = m_allocator.Allocate(aSize);

            if (pData)
                m_usedMemory += m_allocator.Size(pData);

            return pData;
        }

        void Free(void* apData) noexcept override
        {
            m_usedMemory -= m_allocator.Size(apData);
        }

        [[nodiscard]]size_t Size(void* apData) noexcept override
        {
            return m_allocator.Size(apData);
        }

        [[nodiscard]] size_t GetUsedMemory() const noexcept
        {
            return m_usedMemory;
        }

    private:

        T m_allocator;
        size_t m_usedMemory{ 0 };
    };
}
