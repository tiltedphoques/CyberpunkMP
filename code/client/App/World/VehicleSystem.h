#pragma once

#include "Core/Stl.hpp"
#include "RED4ext/Scripting/Natives/Generated/Vector4.hpp"
#include "RED4ext/Scripting/Natives/Generated/Quaternion.hpp"

struct VehicleSystem : RED4ext::IScriptable
{
    RTTI_IMPL_TYPEINFO(VehicleSystem)
    RTTI_IMPL_ALLOCATOR();

    void Update(uint64_t aTick);

    void OnInitialize(const RED4ext::JobHandle& aJob);
    void OnWorldAttached(RED4ext::world::RuntimeScene* aScene);
    void OnAfterWorldDetach();
    void OnDisconnected();

    std::optional<uint64_t> GetVehicleRemoteId() const;
    std::optional<Red::EntityID> GetVehicleGameId() const;

protected:

    void OnVehicleEnter(Red::EntityID aVehicle, const Red::TweakDBID& aVehicleTdbid, Red::CName aName, const Red::Vector4& aPostion, const Red::Quaternion& aOrientation);
    void OnVehicleExit();
    void OnVehicleReady(const Red::EntityID& vehicle);

    bool HandleVehicleLoadMessage(const PacketEvent<server::NotifyVehicleLoad>& aMessage);
    bool HandleVehicleEnterMessage(const PacketEvent<server::NotifyVehicleEnter>& aMessage);
    bool HandleVehicleExitMessage(const PacketEvent<server::NotifyVehicleExit>& aMessage);
    bool HandleVehicleControlMessage(const PacketEvent<server::NotifyVehicleControlAssigned>& aMessage);

    void DoMount(flecs::entity aCharacter, Red::EntityID aVehicle, Red::CName aSit);

private:
    bool m_ready{false};
    Core::Map<Red::EntityID, Vector<PacketEvent<server::NotifyVehicleEnter>>> m_pendingMounts;
    Red::CBaseFunction* m_pSpawnVehicle;
    Red::CBaseFunction* m_pEnterVehicle;
    Red::CBaseFunction* m_pExitVehicle;
    std::optional<uint64_t> m_vehicleRemoteId;
    std::optional<Red::EntityID> m_vehicleGameId;
};

RTTI_DEFINE_CLASS(VehicleSystem, { 
    RTTI_ALIAS("CyberpunkMP.World.VehicleSystem");
    RTTI_METHOD(OnVehicleEnter);
    RTTI_METHOD(OnVehicleExit);
    RTTI_METHOD(OnVehicleReady);
});