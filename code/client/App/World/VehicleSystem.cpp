
#include "VehicleSystem.h"

#include "App/Network/NetworkService.h"
#include "RED4ext/Scripting/Natives/Generated/game/Puppet.hpp"
#include "RED4ext/Scripting/Natives/gameIEntityStubSystem.hpp"
#include "RED4ext/Scripting/Natives/Generated/game/EntityStubComponentPS.hpp"
#include "RED4ext/Scripting/Natives/Generated/vehicle/WheeledBaseObject.hpp"

#include "NetworkWorldSystem.h"
#include "Game/Utils.h"
#include <Launcher/Launcher.h>
#include "App/Components/EntityComponent.h"
#include "App/Components/AttachedComponent.h"
#include "App/Components/SpawningComponent.h"
#include "App/Components/InterpolationComponent.h"


void VehicleSystem::OnWorldAttached(RED4ext::world::RuntimeScene* aScene)
{
    m_ready = true;
    Red::CallVirtual(this, "OnWorldAttached");
}

void VehicleSystem::OnAfterWorldDetach()
{
    m_ready = false;
}

void VehicleSystem::OnDisconnected()
{
    m_vehicleRemoteId = std::nullopt;
    m_vehicleGameId = std::nullopt;
}

void VehicleSystem::OnInitialize(const RED4ext::JobHandle& aJob)
{
    const auto pNetworkService = Core::Container::Get<NetworkService>();
    pNetworkService->RegisterHandler<&VehicleSystem::HandleVehicleLoadMessage>(this);
    pNetworkService->RegisterHandler<&VehicleSystem::HandleVehicleEnterMessage>(this);
    pNetworkService->RegisterHandler<&VehicleSystem::HandleVehicleExitMessage>(this);
    pNetworkService->RegisterHandler<&VehicleSystem::HandleVehicleControlMessage>(this);

    m_pSpawnVehicle = Red::Detail::GetFunction(GetType(), "SpawnVehicle");
    m_pEnterVehicle = Red::Detail::GetFunction(GetType(), "EnterVehicle");
    m_pExitVehicle = Red::Detail::GetFunction(GetType(), "ExitVehicle");
}

std::optional<uint64_t> VehicleSystem::GetVehicleRemoteId() const
{
    return m_vehicleRemoteId;
}

std::optional<Red::EntityID> VehicleSystem::GetVehicleGameId() const
{
    return m_vehicleGameId;
}

void VehicleSystem::OnVehicleEnter(Red::EntityID aVehicle, const Red::TweakDBID& aVehicleTdbid, Red::CName aName, const Red::Vector4& aPostion, const Red::Quaternion& aOrientation)
{
    spdlog::info("[VehicleSystem] OnVehicleEnter");
    const auto pNetworkService = Core::Container::Get<NetworkService>();
    if (!pNetworkService->IsConnected())
        return;

    const auto handle = Red::GetGameSystem<NetworkWorldSystem>();

    client::EnterVehicleRequest request;
    request.set_id(*handle->GetRemotePlayerId());
    request.set_vehicle_id(aVehicleTdbid.value);
    request.set_sit_id(aName.hash);

    const auto serverVehicle = handle->FindEntity(aVehicle);
    if (serverVehicle)
    {
        request.set_remote_vehicle_id(serverVehicle);

        m_vehicleGameId = std::nullopt;
    }
    else
    {
        const auto cEntityRotation = eulerAngles(Game::ToGlm(aOrientation));

        common::Vector3 position;
        position.set_x(aPostion.X);
        position.set_y(aPostion.Y);
        position.set_z(aPostion.Z);
        request.set_position(position);
        request.set_rotation(cEntityRotation.z);

        m_vehicleGameId = aVehicle;
    }

    pNetworkService->Send(request);
}

void VehicleSystem::OnVehicleExit()
{
    m_vehicleGameId = std::nullopt;
    m_vehicleRemoteId = std::nullopt;

    spdlog::info("[VehicleSystem] OnVehicleExit");
    const auto pNetworkService = Core::Container::Get<NetworkService>();
    if (!pNetworkService->IsConnected())
        return;

    client::ExitVehicleRequest request;

    const auto handle = Red::GetGameSystem<NetworkWorldSystem>();
    request.set_id(*handle->GetRemotePlayerId());

    pNetworkService->Send(request);
}

bool VehicleSystem::HandleVehicleLoadMessage(const PacketEvent<server::NotifyVehicleLoad>& aMessage)
{
    spdlog::info("[VehicleSystem] HandleVehicleLoadMessage");
    const auto handle = Red::Handle(this);
    Red::EntityID id;
    Red::ScriptGameInstance game;

    Red::Vector4 position;
    position.X = aMessage.get_position().get_x();
    position.Y = aMessage.get_position().get_y();
    position.Z = aMessage.get_position().get_z();

    const auto eulerAngles = glm::vec3(0.f, 0.f, aMessage.get_rotation());
    const auto quat = glm::quat(eulerAngles);
    Red::Quaternion rotation = Game::ToRed(quat);

    if (!Red::Detail::CallFunctionWithArgs(m_pSpawnVehicle, handle, id, aMessage.get_tweak_id(), position, rotation))
        return false;

    if (!id.IsDynamic())
        return false;

    // spdlog::info("[VehicleSystem] * Spawned: {}, {}", aMessage.get_id(), id.hash);
    const auto worldSystem = Red::GetGameSystem<NetworkWorldSystem>();
    worldSystem->make_alive(aMessage.get_id()).emplace<SpawningComponent>(id);

    return true;
}

bool VehicleSystem::HandleVehicleEnterMessage(const PacketEvent<server::NotifyVehicleEnter>& aMessage)
{
    spdlog::info("[VehicleSystem] HandleVehicleEnterMessage");

    const auto worldSystem = Red::GetGameSystem<NetworkWorldSystem>();

    const auto sit = Red::CName(aMessage.get_sit_id());
    const auto character = worldSystem->GetEntityByServerId(aMessage.get_character_id());

    if (m_vehicleRemoteId && aMessage.get_vehicle_id() == *m_vehicleRemoteId)
    {
        DoMount(character, *m_vehicleGameId, sit);
    }
    else
    {
        const auto entity = worldSystem->GetEntityByServerId(aMessage.get_vehicle_id());
        if (entity.has<EntityComponent>())
        {
            DoMount(character, entity.get<EntityComponent>()->Id, sit);
        }
        else
        {
            // spdlog::info("[VehicleSystem] * Queueing vehicle, server: {}", aMessage.get_vehicle_id());
            const auto vehicle = worldSystem->GetEntityIdByServerId(aMessage.get_vehicle_id());
            // spdlog::info("[VehicleSystem]                     entity: {}", vehicle.hash);
            m_pendingMounts[vehicle].push_back(aMessage);
        }
    }

    // 
    // const auto sit = Red::CName(aMessage.get_sit_id());

    // if (!Red::Detail::CallFunctionWithArgs(m_pEnterVehicle, handle, res, character, vehicle, sit))
    //     return false;

    return true;
}

void VehicleSystem::OnVehicleReady(const Red::EntityID& aVehicleEntityId)
{
    spdlog::info("[VehicleSystem] OnVehicleReady");

    if (m_pendingMounts.find(aVehicleEntityId) != m_pendingMounts.end())
    {
        const auto worldSystem = Red::GetGameSystem<NetworkWorldSystem>();

        for (auto& message : m_pendingMounts[aVehicleEntityId])
        {
            const auto sit = Red::CName(message.get_sit_id());
            const auto character = worldSystem->GetEntityByServerId(message.get_character_id());

            auto vehicleEntity = worldSystem->GetEntityByServerId(message.get_vehicle_id());
            vehicleEntity.emplace<EntityComponent>(aVehicleEntityId, true, nullptr);
            vehicleEntity.remove<SpawningComponent>();

            const auto vehicle = worldSystem->GetEntityIdByServerId(message.get_vehicle_id());

            DoMount(character, vehicle, sit);
        }

        m_pendingMounts.erase(aVehicleEntityId);
    }
    else
    {
        spdlog::info("[VehicleSystem] * Couldn't find vehicle: {}", aVehicleEntityId.hash);
    }
}

bool VehicleSystem::HandleVehicleExitMessage(const PacketEvent<server::NotifyVehicleExit>& aMessage)
{
    spdlog::info("[VehicleSystem] HandleVehicleExitMessage");

    const auto worldSystem = Red::GetGameSystem<NetworkWorldSystem>();
    const auto handle = Red::Handle(this);
    bool res;
    const auto character = worldSystem->GetEntityIdByServerId(aMessage.get_character_id());
    Red::Detail::CallFunctionWithArgs(m_pExitVehicle, handle, res, character);

    auto characterEntity = worldSystem->GetEntityByServerId(aMessage.get_character_id());

    characterEntity.remove<AttachedComponent>();

    return true;
}

bool VehicleSystem::HandleVehicleControlMessage(const PacketEvent<server::NotifyVehicleControlAssigned>& aMessage)
{
    m_vehicleRemoteId = aMessage.get_vehicle_id();
    return true;
}

void VehicleSystem::DoMount(flecs::entity aCharacter, Red::EntityID aVehicle, Red::CName aSit)
{
    const auto worldSystem = Red::GetGameSystem<NetworkWorldSystem>();
    const auto character = worldSystem->GetEntityIdByServerId(aCharacter);
    const auto vehicle = Red::Cast<Red::vehicle::WheeledBaseObject>(worldSystem->GetEntity(aVehicle));
    const auto handle = Red::Handle(this);
    bool res;

    Red::Detail::CallFunctionWithArgs(m_pEnterVehicle, handle, res, character, vehicle->id, aSit);

    aCharacter.add<AttachedComponent>();

    if (!m_vehicleGameId || *m_vehicleGameId != aVehicle)
    {
        // called from vehicle::actions::DriveAction::OnStart
        // static Core::RawFunc<4018412273UL, void (*)(Red::move::Component *, IMoveController &)> AttachLocomotionController
        static Core::RawFunc<4039776020UL, void (*)(Red::vehicle::BaseObject*, bool)> SetIsPlayerControlled;
        static Core::RawFunc<1620777158UL, void (*)(Red::vehicle::BaseObject*, uint32_t)> SetFlags;
        static Core::RawFunc<1585713002UL, void (*)(Red::vehicle::BaseObject*, bool)> SetKinematic;

        // AttachLocomotionController(component, controller);
        SetIsPlayerControlled(vehicle, false);
        // turn on engine
        reinterpret_cast<void (*)(Red::vehicle::WheeledBaseObject*, bool)>(*(uintptr_t*)(*(uintptr_t*)vehicle.instance + 0x328))(vehicle, true);
        vehicle->engineData->unk61 = 0;
        SetFlags(vehicle, 0x10);
        SetFlags(vehicle, 0x80);
        SetKinematic(vehicle, true);
    }
}

