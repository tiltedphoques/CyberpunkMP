#include "Level.h"

#include <Components/LevelTag.h>
#include <Components/MovementComponent.h>
#include <Components/PlayerComponent.h>
#include <Components/AppearanceComponent.h>
#include <Components/AttachmentComponent.h>
#include "Components/CellComponent.h"
#include <Components/CharacterComponent.h>
#include <Components/VehicleComponent.h>

#include "GameServer.h"
#include "World.h"


constexpr static float sCellSize = 60 * 100;
constexpr static int16_t sCellLoadRadius = 3;
constexpr static int16_t sCellUnloadRadius = 4;

GridCell::TPosition Level::ToCell(const glm::vec3& acLocation) noexcept
{
    return {static_cast<int16_t>(acLocation.x / sCellSize), static_cast<int16_t>(acLocation.y / sCellSize)};
}

Level::Level(World* apWorld) noexcept
    : m_pWorld(apWorld)
{
    GServer->RegisterHandler<&Level::HandleSpawnCharacterRequest>(this);
    GServer->RegisterHandler<&Level::HandleMoveEntityRequest>(this);
    GServer->RegisterHandler<&Level::HandleEnterVehicleRequest>(this);
    GServer->RegisterHandler<&Level::HandleExitVehicleRequest>(this);

    m_updateSystem = m_pWorld->system<const LevelActorTag>("Level Update")
        .each([this](flecs::entity aEntity, const LevelActorTag&)
        {
            Update(aEntity);
        });

    m_updateSystem.child_of(m_pWorld->entity("systems"));
}

Level::Level(Level&& aLevel) noexcept
    : m_cells(std::exchange(aLevel.m_cells, {}))
    , m_pWorld(std::exchange(aLevel.m_pWorld, nullptr))
{
}

Level& Level::operator=(Level&& aLevel) noexcept
{
    std::swap(m_pWorld, aLevel.m_pWorld);
    std::swap(m_cells, aLevel.m_cells);

    return *this;
}

void Level::Add(flecs::entity aEntity) noexcept
{
    if (!aEntity)
        return;

    if(aEntity.has<PlayerComponent>())
    {
        AddPlayer(aEntity);
        return;
    }

    aEntity.add<LevelActorTag>();

    auto* pMovementComponent = aEntity.get<MovementComponent>();
    if (!pMovementComponent)
    {
        spdlog::error("Attempt to add entity to level without a movement component!");
        return;
    }

    const auto cellPosition = ToCell(pMovementComponent->Position);
    const auto pCell = GetCell(cellPosition);

    server::NotifyCharacterLoad load = Serialize(aEntity);

    flecs::entity owner = aEntity.parent();

    GetWorld()->each([this, &load, owner](flecs::entity aEntity, const PlayerComponent& aPlayerComponent, const LevelSystemTag&)
    {
        if (!IsDebug() && owner == aEntity)
            return;

        GServer->Send(aPlayerComponent.Connection, load);
    });

    pCell->Add(aEntity);
}

void Level::Remove(flecs::entity aEntity) noexcept
{
    if (!aEntity)
        return;

    if (aEntity.has<PlayerComponent>())
    {
        RemovePlayer(aEntity);
        return;
    }

    server::NotifyEntityUnload unload;
    unload.set_id(aEntity);

    flecs::entity owner = aEntity.parent();

    GetWorld()->each([this, &unload, owner](flecs::entity aEntity, const PlayerComponent& aPlayerComponent)
    {
        if (!IsDebug() && owner == aEntity)
            return;

        GServer->Send(aPlayerComponent.Connection, unload);
    });

    if (auto* pCellComponent = aEntity.get<CellComponent>(); pCellComponent)
    {
        auto& pCell = pCellComponent->pCell;

        pCell->Remove(aEntity);

        if (pCell->Count() == 0)
        {
            m_cells.erase(pCell->GetPosition());
        }
    }

    aEntity.destruct();
}


void Level::Update(flecs::entity aEntity) noexcept
{
}

void Level::AddPlayer(flecs::entity aEntity) noexcept
{
    aEntity.add<LevelSystemTag>();

    auto* pPlayerComponent = aEntity.get<PlayerComponent>();
    if (!pPlayerComponent)
        return;

    Add(pPlayerComponent->Puppet);

    GetWorld()->each(
        [this, player = aEntity, pPlayerComponent](flecs::entity aEntity, const LevelActorTag&)
        {
            if (aEntity.parent() == player)
                return;

            GServer->Send(pPlayerComponent->Connection, Serialize(aEntity));
        });
}

void Level::RemovePlayer(flecs::entity aEntity) noexcept
{
    aEntity.remove<LevelSystemTag>();
}

void Level::HandleSpawnCharacterRequest(PacketEvent<client::SpawnCharacterRequest>& aMessage) noexcept
{
    auto player = GetWorld()->get<PlayerManager>()->GetByConnectionId(aMessage.ConnectionId);

    glm::vec3 pos = {aMessage.get_position().get_x(), aMessage.get_position().get_y(), aMessage.get_position().get_z()};
    glm::vec3 rot = {0.f, 0.f, aMessage.get_rotation()};
    const Vector<String> equipment = aMessage.get_equipment();
    const Vector<uint8_t> ccstate = aMessage.get_ccstate();

    server::SpawnCharacterResponse response;
    response.set_cookie(aMessage.get_cookie());

    auto* pComponent = player.get_mut<PlayerComponent>();
    if (!pComponent)
    {
        spdlog::warn("Received a spawn character request from player who doesn't have a player component!");

        GServer->Send(aMessage.ConnectionId, response);
        return;
    }

    if (IsDebug())
    {
        pos += glm::vec3(2, 0, 0);
        rot += glm::vec3(0, 0, 3.1415);
    }

    pComponent->Puppet = GetWorld()->entity()
        .child_of(player)
        .set<MovementComponent>({pos, rot, {}})
        .set<AppearanceComponent>({equipment, ccstate});

    response.set_id(pComponent->Puppet);
    GServer->Send(aMessage.ConnectionId, response);

    Add(player);
}

void Level::HandleMoveEntityRequest(PacketEvent<client::MoveEntityRequest>& aMessage) noexcept
{
    flecs::entity target(GetWorld()->get_world(), aMessage.get_id());

    if (!target)
    {
        spdlog::warn("Attempt to move invalid entity {:x} from connection {:x}", aMessage.get_id(), aMessage.ConnectionId);
        return;
    }

    const auto player = target.parent();
    if (!player)
    {
        spdlog::warn("Attempt to move an entity without an owner from connection {:x}", aMessage.ConnectionId);
        return;
    }

    auto* pPlayer = player.get<PlayerComponent>();
    if (!pPlayer)
    {
        spdlog::warn("The entity's owner is not a player! From connection {:x}", aMessage.ConnectionId);
        return;
    }

    if (pPlayer->Connection != aMessage.ConnectionId)
    {
        spdlog::warn("The entity's owner is not the current player! From connection {:x}", aMessage.ConnectionId);
        return;
    }

    MovementComponent component;

    auto& pos = aMessage.get_position();

    if (aMessage.has_full_rotation())
    {
        component.Rotation = {aMessage.get_full_rotation().get_x(), aMessage.get_full_rotation().get_y(), aMessage.get_full_rotation().get_z()};
    }
    else
    {
        auto& rot = aMessage.get_rotation();

        component.Rotation = {0.f, 0.f, rot};
    }

    component.Position = {pos.get_x(), pos.get_y(), pos.get_z()};
    component.Velocity = aMessage.get_speed();
    component.Tick = aMessage.get_tick();

    if constexpr (IsDebug())
    {
        component.Position += glm::vec3(5, 0, 0);
        if (!target.has<VehicleComponent>())
            component.Rotation += glm::vec3(0, 0, 3.1415);
    }

    target.set(component);
}

void Level::HandleEnterVehicleRequest(PacketEvent<client::EnterVehicleRequest>& aMessage) noexcept
{
    flecs::entity target(GetWorld()->get_world(), aMessage.get_id());

    if (!target)
    {
        spdlog::warn("Attempt to enter vehicle from invalid entity from connection {:x}", aMessage.ConnectionId);
        return;
    }

    const auto player = target.parent();
    if (!player)
    {
        spdlog::warn("Attempt to enter vehicle an entity without an owner from connection {:x}", aMessage.ConnectionId);
        return;
    }

    auto* pPlayer = player.get<PlayerComponent>();
    if (!pPlayer)
    {
        spdlog::warn("The entity's owner is not a player! From connection {:x}", aMessage.ConnectionId);
        return;
    }

    if (pPlayer->Connection != aMessage.ConnectionId)
    {
        spdlog::warn("The entity's owner is not the current player! From connection {:x}", aMessage.ConnectionId);
        return;
    }

    flecs::entity vehicle;

    if (aMessage.has_remote_vehicle_id())
    {
        vehicle = flecs::entity(GetWorld()->get_world(), aMessage.get_remote_vehicle_id());
        if (!vehicle)
        {
            spdlog::warn("Attempt to enter invalid vehicle {:x} from connection {:x}", aMessage.get_remote_vehicle_id(), aMessage.ConnectionId);
            return;
        }

        spdlog::info("Player {:x} entered vehicle {:x}", aMessage.get_id(), vehicle.id());
    }
    else
    {
        glm::vec3 pos = {aMessage.get_position().get_x(), aMessage.get_position().get_y(), aMessage.get_position().get_z()};
        glm::vec3 rot = {0.f, 0.f, aMessage.get_rotation()};

        if (IsDebug())
        {
            pos += glm::vec3(5, 0, 0);
        }

        vehicle = GetWorld()->entity().child_of(player).set<MovementComponent>({pos, rot, {}}).set<VehicleComponent>({aMessage.get_vehicle_id()});

        spdlog::info("Player {:x} spawned and entered vehicle {:x}", aMessage.get_id(), vehicle.id());
    }

    target.set<AttachmentComponent>({vehicle, aMessage.get_sit_id()});
}

void Level::HandleExitVehicleRequest(PacketEvent<client::ExitVehicleRequest>& aMessage) noexcept
{
    flecs::entity target(GetWorld()->get_world(), aMessage.get_id());

    if (!target)
    {
        spdlog::warn("Attempt to exit vehicle from invalid entity from connection {:x}", aMessage.ConnectionId);
        return;
    }

    const auto player = target.parent();
    if (!player)
    {
        spdlog::warn("Attempt to exit vehicle an entity without an owner from connection {:x}", aMessage.ConnectionId);
        return;
    }

    auto* pPlayer = player.get<PlayerComponent>();
    if (!pPlayer)
    {
        spdlog::warn("The entity's owner is not a player! From connection {:x}", aMessage.ConnectionId);
        return;
    }

    if (pPlayer->Connection != aMessage.ConnectionId)
    {
        spdlog::warn("The entity's owner is not the current player! From connection {:x}", aMessage.ConnectionId);
        return;
    }

    //if (auto* pAttachment = target.get<AttachmentComponent>())
    //    pAttachment->Parent.destruct();

    // Start interpolation again
    target.remove<AttachmentComponent>();
}

server::NotifyCharacterLoad Level::Serialize(flecs::entity aEntity) noexcept
{
    server::NotifyCharacterLoad message;

    if (auto* pMovementComponent = aEntity.get<MovementComponent>())
    {
        common::Vector3 pos;
        pos.set_x(pMovementComponent->Position.x);
        pos.set_y(pMovementComponent->Position.y);
        pos.set_z(pMovementComponent->Position.z);

        message.set_position(pos);
        message.set_rotation(pMovementComponent->Rotation.z);
    }

    if (auto* pCharacterComponent = aEntity.get<CharacterComponent>())
    {
        message.set_is_player(pCharacterComponent->IsPlayerPuppet);
    }

    if (auto* pAppearanceComponent = aEntity.get<AppearanceComponent>())
    {
        message.set_equipment(pAppearanceComponent->equipment);
        message.set_ccstate(pAppearanceComponent->ccstate);
    }

    message.set_id(aEntity);

    return message;
}

void Level::TransferCell(flecs::entity aEntity, GridCell* apOldCell, GridCell* apNewCell) noexcept
{
    Set<GridCell*> cellsToLoad;
    Set<GridCell*> cellsToUnload;

    apOldCell->Remove(aEntity);

    CollectCells(apNewCell, apOldCell, cellsToLoad, cellsToUnload);

    {
        server::NotifyCharacterLoad message;
        message.set_id(aEntity.id());

        for (const auto pCell : cellsToLoad)
        {
        //    pCell->ForEach([&message](flecs::entity aEntity) { apPlayer->Send(message); });
        }
    }

    {
        server::NotifyEntityUnload message;
        message.set_id(aEntity.id());

        //for (const auto pCell : cellsToUnload)
        {
        //    pCell->ForEachPlayer([&message](const Player* apPlayer) { apPlayer->Send(message); });
        }
    }

    apNewCell->Add(aEntity);

    if (apOldCell->Count() == 0)
        m_cells.erase(apOldCell->GetPosition());
}


/*void Level::SendToRelevant(Player* apCharacter, const ServerMessage& acMessage)
{
    ForEachInRange(
        apCharacter->GetCell(), sCellLoadRadius,
        [&acMessage, apCharacter](const Player* apPlayer)
        {
#ifndef DEBUG
            if (apCharacter == apPlayer)
                return;
#endif
            apPlayer->Send(acMessage);
        });
}*/

gsl::not_null<GridCell*> Level::GetCell(const GridCell::TPosition aPosition) noexcept
{
    auto itor = m_cells.find(aPosition);
    if (itor == std::end(m_cells))
    {
        const auto insertRes = m_cells.emplace(aPosition, MakeUnique<GridCell>(aPosition));
        itor = insertRes.first;
    }

    return itor.value().get();
}

void Level::CollectCells(const GridCell* apNewCell, const GridCell* apOldCell, Set<GridCell*>& aToLoad, Set<GridCell*>& aToUnload) noexcept
{
    // Gather all cells we need loaded
    ForEachCellInRange(apNewCell, sCellLoadRadius, [&aToLoad](GridCell* apCell) { aToLoad.insert(apCell); });

    // Remove all cells already loaded from old cell
    ForEachCellInRange(
        apOldCell, sCellLoadRadius,
        [&aToLoad, &aToUnload](GridCell* apCell)
        {
            if (!aToLoad.contains(apCell))
            {
                aToUnload.insert(apCell);
            }
            else
                aToLoad.erase(apCell);
        });
}

void Level::Test() noexcept
{
    for (auto i = -10; i < 10; ++i)
        for (auto j = -10; j < 10; ++j)
            TP_UNUSED(GetCell(GridCell::TPosition(i, j)));

    auto pCell = GetCell(GridCell::TPosition(0, 0));
    assert(pCell == GetCell(GridCell::TPosition(0, 0)));
    assert(pCell != GetCell(GridCell::TPosition(1, 0)));
}
