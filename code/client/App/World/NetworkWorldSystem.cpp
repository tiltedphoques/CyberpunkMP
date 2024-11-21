#include "NetworkWorldSystem.h"

#include "App/Network/NetworkService.h"
#include "RED4ext/Scripting/Natives/Generated/game/Puppet.hpp"
#include "RED4ext/Scripting/Natives/Generated/vehicle/BaseObject.hpp"
#include "RED4ext/Scripting/Natives/gameIEntityStubSystem.hpp"
#include "RED4ext/Scripting/Natives/Generated/game/EntityStubComponentPS.hpp"
#include <RED4ext/Scripting/Natives/Generated/game/mounting/MountingFacility.hpp>
#include <RED4ext/Scripting/Natives/Generated/game/mounting/MountingInfo.hpp>
#include <RED4ext/Scripting/Natives/Generated/vehicle/MoveSystem.hpp>

#include "App/Components/EntityComponent.h"
#include "App/Components/SpawningComponent.h"
#include "App/Components/InterpolationComponent.h"
#include "Game/Utils.h"
#include "Game/CharacterCustomizationSystem.h"
#include <Launcher/Launcher.h>

#include "ChatSystem.h"

static uint64_t GTick = 0;

uint64_t NetworkWorldSystem::GetTick()
{
    return GTick;
}

NetworkWorldSystem::NetworkWorldSystem()
{
    set_entity_range(10'000'000, 20'000'000);
}

bool NetworkWorldSystem::Spawn(uint64_t aServerId, const Red::Vector4& aPosition, const Red::Quaternion& aRotation, const Red::DynArray<Red::TweakDBID>& aEquipment, const Vector<uint8_t> aCcstate)
{
    if (!m_ready)
        return false;

    const auto handle = Red::GetGameSystem<NetworkWorldSystem>();
    Red::EntityID id;
    Red::ScriptGameInstance game;

    Red::Handle<game::ui::CharacterCustomizationState> stateHandle;
    CreateHandle_CharacterCustomizationState(&stateHandle);

    auto reader = CMPReader(aCcstate);
    CharacterCustomizationState_Serialize(stateHandle.instance, &reader);

    if (!Red::Detail::CallFunctionWithArgs(m_pCreatePuppet, handle, id, aPosition, aRotation, stateHandle.instance->isBodyGenderMale))
        return false;
    
    auto apprSystem = Red::GetGameSystem<NetworkWorldSystem>()->GetAppearanceSystem();
    apprSystem->AddEntity(id, aEquipment, aCcstate);

    if (!id.IsDynamic())
        return false;

    ensure(aServerId).emplace<SpawningComponent>(id);

    return true;
}

void NetworkWorldSystem::DeSpawn(uint64_t aServerId) const
{
    const auto entity = GetEntityByServerId(aServerId);

    if (!entity)
        return;

    if (auto* pEntity = entity.get<EntityComponent>())
    {
        const auto handle = Red::GetGameSystem<NetworkWorldSystem>();
        Red::Detail::CallFunctionWithArgs(m_pDeletePuppet, handle, pEntity->Id);
    }
    else if (auto* pEntity = entity.get<SpawningComponent>())
    {
        const auto handle = Red::GetGameSystem<NetworkWorldSystem>();
        Red::Detail::CallFunctionWithArgs(m_pDeletePuppet, handle, pEntity->Id);
    }

    entity.destruct();
}

Red::Handle<Red::Entity> NetworkWorldSystem::GetEntity(Red::EntityID aId) const
{
    Red::Handle<Red::IGameSystem> dynamicEntitySystem;
    if (!Red::CallStatic("ScriptGameInstance", "GetDynamicEntitySystem", dynamicEntitySystem))
        return nullptr;

    Red::Handle<Red::Entity> entity;
    if (!Red::CallVirtual(dynamicEntitySystem, "GetEntity", entity, aId))
        return nullptr;

    return entity;
}

flecs::entity NetworkWorldSystem::GetEntityByServerId(uint64_t aServerId) const
{
    return flecs::entity(*this, aServerId);
}

Red::EntityID NetworkWorldSystem::GetEntityIdByServerId(uint64_t aServerId) const
{
    const auto entity = GetEntityByServerId(aServerId);
    if (!entity)
        return 0;

    if (auto* pEntity = entity.get<EntityComponent>())
        return pEntity->Id;

    if (auto* pEntity = entity.get<SpawningComponent>())
        return pEntity->Id;

    return 0;
}

flecs::entity NetworkWorldSystem::FindEntity(Red::EntityID aId) const
{
    auto entity = query<EntityComponent>().find(
        [aId](const EntityComponent& component)
        {
            return component.Id == aId;
        });

    if (!entity)
    {
        entity = query<SpawningComponent>().find(
            [aId](const SpawningComponent& component)
            {
                return component.Id == aId;
            });
    }

    return entity;
}

void NetworkWorldSystem::Update(uint64_t aTick)
{
    GTick = aTick;

    const auto delta = std::min(aTick - m_lastTick, 1000ull);
    m_lastTick = aTick;

    const auto service = Core::Container::Get<NetworkService>();
    if (service && service->IsConnected())
        progress(static_cast<float>(delta) / 1000.f);
}

void NetworkWorldSystem::OnWorldAttached(RED4ext::world::RuntimeScene* aScene)
{    
    spdlog::info("[NetworkWorldSystem] OnWorldAttached");
    IGameSystem::OnWorldAttached(aScene);

    m_chatSystem->OnWorldAttached(aScene);
    m_appearanceSystem->OnWorldAttached(aScene);
    m_interpolationSystem->OnWorldAttached(aScene);
    m_vehicleSystem->OnWorldAttached(aScene);

    m_ready = true;
}

void NetworkWorldSystem::OnAfterWorldDetach()
{
    spdlog::info("[NetworkWorldSystem] OnAfterWorldDetach");
    m_ready = false;

    m_interpolationSystem->OnAfterWorldDetach();
    m_chatSystem->OnAfterWorldDetach();
    m_vehicleSystem->OnAfterWorldDetach();

    m_remotePlayerId = std::nullopt;

    IGameSystem::OnAfterWorldDetach();
}

void NetworkWorldSystem::OnBeforeWorldDetach(RED4ext::world::RuntimeScene* aScene)
{
    IGameSystem::OnBeforeWorldDetach(aScene);

    m_appearanceSystem->OnBeforeWorldDetach(aScene);
}

void NetworkWorldSystem::HandleCharacterLoad(const PacketEvent<server::NotifyCharacterLoad>& aMessage)
{
    auto& pos = aMessage.get_position();
    auto& rot = aMessage.get_rotation();

    const Red::Vector4 position{pos.get_x(), pos.get_y(), pos.get_z(), 1.f};
    const auto eulerAngles = glm::vec3(0.f, 0.f, rot);
    const auto quat = glm::quat(eulerAngles);

    const Red::Quaternion rotation{quat.x, quat.y, quat.z, quat.w};

    auto equipment = Red::DynArray<Red::TweakDBID>(this->GetAllocator());
    for (auto item : aMessage.get_equipment()) 
    {
        equipment.EmplaceBack(item);
    }

    auto ccstate = aMessage.get_ccstate();

    Spawn(aMessage.get_id(), position, rotation, equipment, ccstate);
}

void NetworkWorldSystem::HandleEntityUnload(const PacketEvent<server::NotifyEntityUnload>& aMessage)
{
    DeSpawn(aMessage.get_id());
}

void NetworkWorldSystem::HandleSpawnCharacterResponse(const PacketEvent<server::SpawnCharacterResponse>& aMessage)
{
    if (!aMessage.has_id())
    {
        spdlog::error("Failed to spawn our character on the server...");
        return;
    }

    SetRemotePlayerId(aMessage.get_id());
}

static Core::RawFunc<
    1160782872UL, 
    bool (*)(Red::game::mounting::MountingFacility *, const Red::ent::Entity &, const Red::game::mounting::MountingSlotId &, bool)> 
    IsMountedToObject;
static Core::RawFunc<
    3120376212UL, 
    bool (*)(Red::game::mounting::MountingFacility *, const Red::ent::Entity &, const Red::game::mounting::MountingSlotId &, Red::game::mounting::MountingInfo &)> 
    GetMountingInfo;

void NetworkWorldSystem::UpdatePlayerLocation() const
{
    const auto system = Red::GetGameSystem<Game::PlayerSystem>();
    Red::Handle<Red::GameObject> player;
    system->GetLocalPlayerControlledGameObject(player);

    if (!player || !GetRemotePlayerId())
        return;

    auto puppet = Red::Cast<Red::game::Puppet>(player);

    // const auto mountingFacility = Red::GetGameSystem<Red::game::mounting::MountingFacility>();
    // Red::game::mounting::MountingInfo mountingInfo;
    // if (GetMountingInfo(mountingFacility, *puppet.instance, Red::CName("seat_front_left"), mountingInfo)) {
    if (auto vehicle_id = GetVehicleSystem()->GetVehicleGameId())
    {
        // auto vehicle = Red::Cast<Red::vehicle::BaseObject>(GetEntity(mountingInfo.parentId));
        auto vehicle = Red::Cast<Red::vehicle::BaseObject>(GetEntity(*vehicle_id));
        if (!vehicle)
        {
            spdlog::info("Couldn't find vehicle: {}", (*vehicle_id).hash);
            return;
        }
        if (auto remote_id = GetVehicleSystem()->GetVehicleRemoteId(); remote_id != std::nullopt)
        {
            auto transform = Red::WorldTransform();

            transform = vehicle->rigidBody->currentTransform;

            // const auto cEntityRotation = eulerAngles(Game::ToGlm(vehicle->placedComponent->worldTransform.Orientation));

            // about the same
            // transform = vehicle->worldTransform;

            // about the same
            // transform = vehicle->runtimeData->transform;

            // try to get transform from move system
            // seems to return 0 :/
            // auto transform = Red::WorldTransform();
            // const auto moveSystem = Red::GetGameSystem<Red::vehicle::MoveSystem>();
            // // GetCurrentTransform
            // reinterpret_cast<void (*)(const Red::vehicle::MoveSystem *, const Red::EntityID &, Red::WorldTransform*)>(*(uintptr_t*)(*(uintptr_t*)moveSystem + 0x250))(moveSystem, vehicle->id, &transform);

            const auto cEntityPosition = transform.Position;
            const auto cEntityRotation = eulerAngles(Game::ToGlm(transform.Orientation));
            float speed = vehicle->rigidBody->velocity.Magnitude();

            common::Vector3 pos;
            pos.set_x(cEntityPosition.x);
            pos.set_y(cEntityPosition.y);
            pos.set_z(cEntityPosition.z);

            common::Vector3 rot;
            rot.set_x(cEntityRotation.x);
            rot.set_y(cEntityRotation.y);
            rot.set_z(cEntityRotation.z);

            client::MoveEntityRequest request;
            request.set_position(pos);
            request.set_full_rotation(rot);
            request.set_id(*remote_id);
            request.set_speed(speed);
            request.set_tick(GetTick());

            const auto pNetworkService = Core::Container::Get<NetworkService>();
            pNetworkService->Send(request);

            client::MoveEntityRequest characterRequest;
            characterRequest.set_position(pos);
            characterRequest.set_rotation(cEntityRotation.z);
            characterRequest.set_id(*GetRemotePlayerId());
            characterRequest.set_speed(speed);
            characterRequest.set_tick(GetTick());

            pNetworkService->Send(characterRequest);
        }
    }
    else
    {
        const auto cEntityPosition = puppet->placedComponent->localTransform.Position;
        const auto cEntityRotation = eulerAngles(Game::ToGlm(puppet->placedComponent->worldTransform.Orientation));
        float speed = puppet->moveComponent->speed.Magnitude();

        common::Vector3 pos;
        pos.set_x(cEntityPosition.x);
        pos.set_y(cEntityPosition.y);
        pos.set_z(cEntityPosition.z);

        client::MoveEntityRequest request;
        request.set_position(pos);
        request.set_rotation(cEntityRotation.z);
        request.set_id(*GetRemotePlayerId());
        request.set_speed(speed);
        request.set_tick(GetTick());

        const auto pNetworkService = Core::Container::Get<NetworkService>();
        pNetworkService->Send(request);
    }

    // if (GetEntityByServerId(*GetRemotePlayerId()).get_mut<InterpolationComponent>()->Attached)
    // {
    //     auto vehicle_id = GetVehicleSystem()->GetVehicle(player->id);
    //     if (!vehicle_id)
    //     {
    //         spdlog::warn("No vehicle for player({})", player->id.hash);
    //         return;
    //     }
    //     auto entity = GetEntity(vehicle_id);
    //     if (!entity)
    //     {
    //         spdlog::warn("No entity for vehicle({})", vehicle_id.hash);
    //         return;
    //     }
    //     auto vehicle = Red::Cast<Red::vehicle::BaseObject>(entity);
    //     if (!vehicle)
    //     {
    //         spdlog::warn("Entity is not vehicle");
    //         return;
    //     }
    //     else
    //     {
    //         // if (vehicle->placedComponent) {
    //             // entityPosition = vehicle->placedComponent->localTransform.Position;
    //         // } else {
    //             // entityPosition = vehicle->worldTransform.Position;
    //             entityPosition = vehicle->rigidBody->worldPosition;
    //         // }
    //     }
    // }
}

void NetworkWorldSystem::OnInitialize(const RED4ext::JobHandle& aJob)
{
    spdlog::info("[OnInitialize]");

    IGameSystem::OnInitialize(aJob);

    const auto pNetworkService = Core::Container::Get<NetworkService>();
    pNetworkService->RegisterHandler<&NetworkWorldSystem::HandleCharacterLoad>(this);
    pNetworkService->RegisterHandler<&NetworkWorldSystem::HandleEntityUnload>(this);
    pNetworkService->RegisterHandler<&NetworkWorldSystem::HandleSpawnCharacterResponse>(this);

    m_remotePlayerId = std::nullopt;

    m_pCreatePuppet = Red::Detail::GetFunction(GetType(), "CreatePuppet");
    m_pDeletePuppet = Red::Detail::GetFunction(GetType(), "DeletePuppet");

    m_interpolationSystem = RED4ext::MakeHandle<InterpolationSystem>();
    m_interpolationSystem->OnInitialize(aJob);

    m_appearanceSystem = RED4ext::MakeHandle<AppearanceSystem>();
    m_appearanceSystem->OnInitialize(aJob);

    m_chatSystem = RED4ext::MakeHandle<ChatSystem>();
    m_chatSystem->OnInitialize(aJob);

    m_vehicleSystem = RED4ext::MakeHandle<VehicleSystem>();
    m_vehicleSystem->OnInitialize(aJob);
}

void NetworkWorldSystem::Connect(const Red::CString& aAddress)
{
    auto address = fmt::format("{}:{}", launcher::GetLaunchContext()->ip, launcher::GetLaunchContext()->port);
    Core::Container::Get<NetworkService>()->Connect(address);
}

void NetworkWorldSystem::Disconnect()
{
    Core::Container::Get<NetworkService>()->Close();
}

void NetworkWorldSystem::OnConnected()
{
    RED4ext::StackArgs_t args;
    ExecuteFunction(this, this->GetNativeType()->GetFunction("OnConnected"), nullptr, args);

    const auto pNetworkService = Core::Container::Get<NetworkService>();

    m_updatePlayerLocation = system("Update player location")
        .kind(flecs::OnUpdate)
        .interval(1.f / pNetworkService->GetServerSettings().get_update_rate())
        .iter([this](flecs::iter& it)
        {
            UpdatePlayerLocation();
        });

    m_updateSpawningEntities = system<SpawningComponent>("Spawning entity process")
        .interval(0.2f)
        .write<EntityComponent>()
        .write<SpawningComponent>()
        .each([this](flecs::entity aEntity, SpawningComponent& aSpawning)
        {
            const auto pEntity = GetEntity(aSpawning.Id);
            if (const auto pOwner = Red::Cast<Red::GameObject>(pEntity))
            {
                if (pOwner->tags.Contains("CyberpunkMP.Vehicle"))
                    return;

                 aEntity.emplace<EntityComponent>(aSpawning.Id, false, aSpawning.Controller);
                 aEntity.remove<SpawningComponent>();
                 pOwner->tags.Add("CyberpunkMP.Puppet");
            }
        });

    m_interpolationSystem->OnConnected();
}

void NetworkWorldSystem::OnDisconnected(Client::EDisconnectReason aReason)
{
    each([this](flecs::entity entity, EntityComponent&)
        {
            DeSpawn(entity.raw_id());
            entity.destruct();
        });

    each([this](flecs::entity entity, SpawningComponent&)
        {
            DeSpawn(entity.raw_id());
            entity.destruct();
        });

    if (m_updatePlayerLocation)
        m_updatePlayerLocation.destruct();

    if (m_updateSpawningEntities)
        m_updateSpawningEntities.destruct();

    m_interpolationSystem->OnDisconnected();
    m_vehicleSystem->OnDisconnected();

    RED4ext::StackArgs_t args;
    auto reason = (uint32_t)aReason;
    args.emplace_back(RED4ext::CRTTISystem::Get()->GetType("Uint32"), &reason);
    ExecuteFunction(this, this->GetNativeType()->GetFunction("OnDisconnected"), nullptr, args);
}
