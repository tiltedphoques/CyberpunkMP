#pragma once

#include "VehicleComponent.h"
#include "MovementComponent.h"
#include "PlayerComponent.h"

#include "GameServer.h"

void ReplicateVehicleSpawn(flecs::entity aEntity, const VehicleComponent& aVehicleComponent, const MovementComponent& aMovementComponent)
{
    common::Vector3 pos;
    pos.set_x(aMovementComponent.Position.x);
    pos.set_y(aMovementComponent.Position.y);
    pos.set_z(aMovementComponent.Position.z);

    server::NotifyVehicleLoad load;
    load.set_position(pos);
    load.set_id(aEntity);
    load.set_rotation(aMovementComponent.Rotation.z);
    load.set_tweak_id(aVehicleComponent.TweakDBID);

    aEntity.world().each(
        [aEntity, &load](flecs::entity player, const PlayerComponent& aPlayerComponent)
        {
            if (!IsDebug() && player == aEntity.parent())
                return;

            GServer->Send(aPlayerComponent.Connection, load);
        });
}

void VehicleComponent::Register(flecs::world& aWorld)
{
    aWorld.component<VehicleComponent>()
        .member("TweakDBID", &VehicleComponent::TweakDBID);

    auto observer = aWorld.observer<const VehicleComponent>("Vehicle spawn replication")
        .with<const MovementComponent>().filter()
        .event(flecs::OnSet)
        .each(
            [](flecs::iter& it, size_t i, const VehicleComponent& aVehicleComponent)
            {
                const auto entity = it.entity(i);

                ReplicateVehicleSpawn(entity, aVehicleComponent, *entity.get<const MovementComponent>());
            });

    observer.child_of(aWorld.entity("observers"));
}
