#pragma once

#include <RED4ext/Scripting/Natives/Generated/move/Component.hpp>

struct AnimationData;

namespace Game
{
using EntityID = Core::OffsetPtr<0x48, Red::EntityID>;
using EntityPtr = Core::OffsetPtr<0x90, Red::Entity*>;

struct Controller
{
    virtual RED4ext::Memory::PoolAI_Movement* GetMemoryPool();
    virtual ~Controller();

    uint32_t m_type = 0; // 4
    bool m_attached = false; // 8
    bool m_highPriotiry = false; // C
    Red::move::Component* MoveComponent; // 10
    Red::EntityID Id;
};

constexpr auto IdleController_SetAnimation = Core::RawFunc<
    /* hash = */ 2268141838UL,
    /* type = */ void (*)(Game::Controller*, AnimationData&)>();

template <class T>
Red::Handle<T> FindComponent(Red::Entity* apEntity)
{
    static auto pClass = Red::ResolveClass<T>();
    if (pClass && apEntity)
    {
        for (auto& component : apEntity->componentsStorage.components)
        {
            if (component && component->GetType()->IsA(pClass))
                return Red::Cast<T>(component);
        }
    }

    return {};
}
}
