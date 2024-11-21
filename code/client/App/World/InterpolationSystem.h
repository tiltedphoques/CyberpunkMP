#pragma once

#include "Core/Hooking/HookingAgent.hpp"
#include "RED4ext/Scripting/Natives/gameIEntityStubSystem.hpp"


struct InterpolationSystem : RED4ext::IScriptable, Core::HookingAgent
{
    RTTI_IMPL_TYPEINFO(InterpolationSystem)
    RTTI_IMPL_ALLOCATOR();

    void OnInitialize(const RED4ext::JobHandle& aJob);
    void OnWorldAttached(RED4ext::world::RuntimeScene* aScene);
    void OnAfterWorldDetach();

    void OnConnected();
    void OnDisconnected();

protected:

    void HandleNotifyEntityMove(const PacketEvent<server::NotifyEntityMove>& aMessage);

private:
    bool m_ready{false};
    flecs::system m_interpolator;
    flecs::observer m_entityObserver;
};

RTTI_DEFINE_CLASS(InterpolationSystem, { 
    RTTI_ALIAS("CyberpunkMP.World.InterpolationSystem");
});