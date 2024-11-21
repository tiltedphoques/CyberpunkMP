#pragma once

#include "Stl.h"


template<class T, class U>
struct Locked;

template<class T, class TLock = std::mutex>
struct Lockable
{
    struct Data
    {
        template<class... Args>
        Data(Args&& ... aArgs)
            : Value(std::forward<Args>(aArgs)...)
        {}

        T Value;
        TLock Lock;
    };

    struct Ref
    {
        Ref(typename SharedPtr<Data>::weak_type apData)
            : m_pData(std::move(apData))
        {}

        [[nodiscard]] Locked<T, TLock> Lock() const
        {
            if (auto locked = m_pData.lock())
            {
                return { locked };
            }

            return { {} };
        }

    private:
        typename SharedPtr<Data>::weak_type m_pData;
    };


    template<class... Args>
    Lockable(Args&& ... aArgs)
        : m_pData(MakeShared<Data>(std::forward<Args>(aArgs)...))
    {}

    Ref AsRef() const noexcept
    {
        return Ref(m_pData);
    }

    [[nodiscard]] Locked<T, TLock> Lock() const
    {
        return AsRef().Lock();
    }

private:

    friend struct Locked<T, TLock>;

    SharedPtr<Data> m_pData;
};


#include "Locked.h"
