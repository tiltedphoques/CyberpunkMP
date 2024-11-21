#pragma once

#include "Meta.h"
#include <cstdint>

#define TP_ALLOCATOR                                                                    \
    void SetAllocator(Allocator* apAllocator) noexcept { m_pAllocator = apAllocator; }  \
    Allocator* GetAllocator() noexcept { return m_pAllocator; }          \
private:                                                                                \
    Allocator* m_pAllocator{ Allocator::Get() };          \
public:

namespace details
{
    template<typename T>
    using has_get_allocator_t = decltype(std::declval<T&>().GetAllocator());

    template<typename T>
    using has_set_allocator_t = decltype(std::declval<T&>().SetAllocator(nullptr));

    template<typename T>
    constexpr bool has_allocator = is_detected_v<has_get_allocator_t, T>&& is_detected_v<has_set_allocator_t, T>;
}

struct Allocator
{
    virtual ~Allocator() = default;
    [[nodiscard]] virtual void* Allocate(size_t aSize) noexcept  = 0;
    virtual void Free(void* apData) noexcept  = 0;
    [[nodiscard]] virtual size_t Size(void* apData) noexcept = 0;
    virtual void Reset() noexcept {};

    using TArraySizePrefix = size_t;

    template<class T>
    [[nodiscard]] T* New() noexcept
    {
        static_assert(alignof(T) <= alignof(details::default_align_t));

        auto pData = static_cast<T*>(Allocate(sizeof(T)));
        if (pData)
        {
            return new (pData) T();
        }

        return nullptr;
    }

    template<class T, std::enable_if_t<std::is_array_v<T>>* = nullptr>
    [[nodiscard]] auto New(TArraySizePrefix aCount) noexcept
    {
        using TUnderlyingType = std::remove_all_extents_t<T>;

        static_assert(alignof(T) <= alignof(details::default_align_t));

        const auto pData = static_cast<uint8_t*>(Allocate(sizeof(TUnderlyingType) * aCount + sizeof(TArraySizePrefix)));
        *reinterpret_cast<TArraySizePrefix*>(pData) = aCount;

        auto pArray = reinterpret_cast<TUnderlyingType*>(pData + sizeof(TArraySizePrefix));

        if (pData)
        {
            for (auto i = 0; i < aCount; ++i)
                new (&pArray[i]) TUnderlyingType();
        }

        return pArray;
    }

    template<class T, std::enable_if_t<!std::is_array_v<T>>* = nullptr, class... Args>
    [[nodiscard]] T* New(Args&&... args) noexcept
    {
        static_assert(alignof(T) <= alignof(details::default_align_t));

        auto pData = static_cast<T*>(Allocate(sizeof(T)));
        if (pData)
        {
            return new (pData) T(std::forward<Args>(args)...);
        }

        return nullptr;
    }

    template<class T, std::enable_if_t<std::is_array_v<T>>* = nullptr, class... Args>
    [[nodiscard]] auto New(TArraySizePrefix aCount, Args&&... args) noexcept
    {
        using TUnderlyingType = std::remove_all_extents_t<T>;

        static_assert(alignof(T) <= alignof(details::default_align_t));

        const auto pData = static_cast<uint8_t*>(Allocate(sizeof(TUnderlyingType) * aCount + sizeof(TArraySizePrefix)));
        *reinterpret_cast<TArraySizePrefix*>(pData) = aCount;

        auto pArray = reinterpret_cast<TUnderlyingType*>(pData + sizeof(TArraySizePrefix));

        if (pData)
        {
            for(auto i = 0; i < aCount; ++i)
                new (&pArray[i]) TUnderlyingType(std::forward<Args>(args)...);
        }

        return pArray;
    }

    template<class T, std::enable_if_t<!std::is_array_v<T>>* = nullptr>
    void Delete(T* apData) noexcept
    {
        if (apData == nullptr)
            return;

        apData->~T();
        Free(apData);
    }

    template<class T, std::enable_if_t<std::is_array_v<T>>* = nullptr>
    void Delete(T apData) noexcept
    {
        if (apData == nullptr)
            return;

        using TUnderlyingType = std::remove_all_extents_t<T>;

        auto* pSize = reinterpret_cast<TArraySizePrefix*>(reinterpret_cast<uint8_t*>(apData) - sizeof(TArraySizePrefix));
        for(auto i = 0ull; i < *pSize; ++i)
        {
            apData[i].~TUnderlyingType();
        }

        Free(pSize);
    }

    static void Set(Allocator* apAllocator = nullptr) noexcept;
    [[nodiscard]] static Allocator* Get() noexcept;
    [[nodiscard]] static Allocator* GetDefault() noexcept;
};

struct AllocatorCompatible
{
    TP_ALLOCATOR
};


struct ScopedAllocator
{
    explicit ScopedAllocator(Allocator* apAllocator) noexcept;
    explicit ScopedAllocator(Allocator& aAllocator) noexcept;
    ~ScopedAllocator() noexcept;

    TP_NOCOPYMOVE(ScopedAllocator);

private:
    Allocator* m_pOldAllocator;
};


struct ScopedResetAllocator
{
    explicit ScopedResetAllocator(Allocator* apAllocator) noexcept;
    explicit ScopedResetAllocator(Allocator& aAllocator) noexcept;
    ~ScopedResetAllocator() noexcept;

    TP_NOCOPYMOVE(ScopedResetAllocator);

private:
    Allocator* m_pOldAllocator;
};

template<class T>
[[nodiscard]] auto New() noexcept
{
    if constexpr (details::has_allocator<std::remove_all_extents_t<T>>)
        return Allocator::Get()->New<T>();
    else
        return Allocator::GetDefault()->New<T>();
}

template<class T, class... Args>
[[nodiscard]] auto New(Args&&... args) noexcept
{
    if constexpr (details::has_allocator<std::remove_all_extents_t<T>>)
        return Allocator::Get()->New<T>(std::forward<Args>(args)...);
    else
        return Allocator::GetDefault()->New<T>(std::forward<Args>(args)...);
}

template<class T, std::enable_if_t<!std::is_array_v<T>>* = nullptr>
void Delete(T* apEntry) noexcept
{
    if constexpr (details::has_allocator<T>)
    {
        apEntry->GetAllocator()->Delete(apEntry);
    }
    else
    {
        Allocator::GetDefault()->Delete(apEntry);
    }
}

template<class T, std::enable_if_t<std::is_array_v<T>>* = nullptr>
void Delete(T apEntry) noexcept
{
    if constexpr (details::has_allocator<std::remove_all_extents_t<T>>)
    {
        apEntry->GetAllocator()->template Delete<T>(apEntry);
    }
    else
    {
        Allocator::GetDefault()->template Delete<T>(apEntry);
    }
}

