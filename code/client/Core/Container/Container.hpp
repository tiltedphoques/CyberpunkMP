#pragma once

#include "Core/Stl.hpp"

namespace Core
{
class Container
{
public:
    template<typename T>
    inline static void Set(const Core::SharedPtr<T>& aInstance)
    {
        Resolver<T>::Assign(aInstance);
    }

    template<typename T>
    inline static Core::SharedPtr<T> Get()
    {
        return Resolver<T>::Retrieve().lock();
    }

    template<typename T>
    inline static bool Has()
    {
        return !Resolver<T>::Retrieve().expired();
    }

    template <typename T>
    inline static void Reset()
    {
        Resolver<T>::Reset();
    }

private:
    template<typename T>
    struct Resolver
    {
        inline static void Assign(const Core::SharedPtr<T>& aInstance)
        {
            s_instance = aInstance;
        }

        inline static Core::WeakPtr<T>& Retrieve()
        {
            return s_instance;
        }

        inline static void Reset()
        {
            s_instance = {};
        }

        inline static Core::WeakPtr<T> s_instance;
    };
};
}
