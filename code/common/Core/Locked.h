#pragma once

#include "Lockable.h"

template<class T, class TLock>
struct Locked
{
    Locked(SharedPtr<typename Lockable<T, TLock>::Data> apData)
        : m_pData(std::move(apData))
        , m_handle(m_pData->Lock)
    {
    }

    operator typename Lockable<T, TLock>::Ref() const
    {
        return { m_pData };
    }

    bool IsValid() const
    {
        return (bool)m_pData;
    }

    operator T& ()
    {
        return Get();
    }

    operator T&() const
    {
        return Get();
    }

    const T& Get() const noexcept
    {
        return m_pData->Value;
    }

    T& Get() noexcept
    {
        return m_pData->Value;
    }

private:
    friend struct Lockable<T, TLock>;

    SharedPtr<typename Lockable<T, TLock>::Data> m_pData;
    std::scoped_lock<TLock> m_handle;
};

