#pragma once

#include "Core/Hooking/HookingAgent.hpp"
#include "RED4ext/Scripting/Natives/Generated/Vector4.hpp"
#include "RED4ext/Scripting/Natives/Generated/Quaternion.hpp"
#include "Network/Client.h"
#include "Red/TypeInfo/Macros/Definition.hpp"
#include "AppearanceSystem.h"
#include "ChatSystem.h"
#include "InterpolationSystem.h"
#include "VehicleSystem.h"

struct NetworkWorldSystem : RED4ext::IGameSystem, Core::HookingAgent, flecs::world
{
    RTTI_IMPL_TYPEINFO(NetworkWorldSystem);
    RTTI_IMPL_ALLOCATOR();

    static uint64_t GetTick();

    NetworkWorldSystem();

    bool Spawn(uint64_t aServerId, const Red::Vector4& aPosition, const Red::Quaternion& aRotation, const Red::DynArray<Red::TweakDBID>& aEquipment, const Vector<uint8_t> aCcstate);
    void DeSpawn(uint64_t aServerId) const;

    Red::Handle<Red::Entity> GetEntity(Red::EntityID aId) const;
    flecs::entity GetEntityByServerId(uint64_t aServerId) const;
    Red::EntityID GetEntityIdByServerId(uint64_t aServerId) const;
    flecs::entity FindEntity(Red::EntityID aId) const;

    void SetRemotePlayerId(uint64_t aId) { m_remotePlayerId = aId; }
    std::optional<uint64_t> GetRemotePlayerId() const { return m_remotePlayerId; }

    Red::Handle<AppearanceSystem> GetAppearanceSystem() const { return m_appearanceSystem; }
    Red::Handle<ChatSystem> GetChatSystem() const { return m_chatSystem; }
    Red::Handle<InterpolationSystem> GetInterpolationSystem() const { return m_interpolationSystem; }
    Red::Handle<VehicleSystem> GetVehicleSystem() const { return m_vehicleSystem; }

    void Update(uint64_t aTick);

    void OnInitialize(const RED4ext::JobHandle& aJob) override;

    void Connect(const Red::CString& aAddress);
    void Disconnect();
    void OnConnected();
    void OnDisconnected(Client::EDisconnectReason);

protected:
    void OnWorldAttached(RED4ext::world::RuntimeScene* aScene) override;
    void OnAfterWorldDetach() override;
    void OnBeforeWorldDetach(RED4ext::world::RuntimeScene* aScene) override;

    void HandleCharacterLoad(const PacketEvent<server::NotifyCharacterLoad>& aMessage);
    void HandleEntityUnload(const PacketEvent<server::NotifyEntityUnload>& aMessage);
    void HandleSpawnCharacterResponse(const PacketEvent<server::SpawnCharacterResponse>& aMessage);

    void UpdatePlayerLocation() const;

private:
    bool m_ready{false};
    Red::CBaseFunction* m_pCreatePuppet;
    Red::CBaseFunction* m_pDeletePuppet;
    std::optional<uint64_t> m_remotePlayerId;
    uint64_t m_lastTick;
    flecs::system m_updatePlayerLocation;
    flecs::system m_updateSpawningEntities;
    Red::Handle<InterpolationSystem> m_interpolationSystem;
    Red::Handle<AppearanceSystem> m_appearanceSystem;
    Red::Handle<ChatSystem> m_chatSystem;
    Red::Handle<VehicleSystem> m_vehicleSystem;
};

RTTI_DEFINE_CLASS(NetworkWorldSystem, { 
    RTTI_ALIAS("CyberpunkMP.World.NetworkWorldSystem");
    RTTI_METHOD(Connect);
    RTTI_METHOD(Disconnect);
    RTTI_METHOD(GetEntityIdByServerId);
    RTTI_METHOD(GetAppearanceSystem);
    RTTI_METHOD(GetInterpolationSystem);
    RTTI_METHOD(GetChatSystem);
    RTTI_METHOD(GetVehicleSystem);
});
