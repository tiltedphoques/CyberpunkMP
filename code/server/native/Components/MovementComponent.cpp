#include "MovementComponent.h"

#include "GameServer.h"
#include "PlayerComponent.h"

void ReplicateMovementComponent(flecs::entity aEntity, const MovementComponent& aComponent)
{
    common::Vector3 pos;
    pos.set_x(aComponent.Position.x);
    pos.set_y(aComponent.Position.y);
    pos.set_z(aComponent.Position.z);

    server::NotifyEntityMove message;
    message.set_id(aEntity);

    if (aComponent.Rotation.x == 0.f && aComponent.Rotation.y == 0.f)
    {
        message.set_rotation(aComponent.Rotation.z);
    }
    else
    {
        common::Vector3 rot;
        rot.set_x(aComponent.Rotation.x);
        rot.set_y(aComponent.Rotation.y);
        rot.set_z(aComponent.Rotation.z);

        message.set_full_rotation(rot);
    }

    message.set_position(pos);
    message.set_tick(aComponent.Tick);
    message.set_speed(aComponent.Velocity);

    aEntity.world().each(
        [aEntity, &message](flecs::entity player, const PlayerComponent& aPlayerComponent)
        {
            if (!IsDebug() && player == aEntity.parent())
                return;

            GServer->Send(aPlayerComponent.Connection, message);
        });
}

void MovementComponent::Register(flecs::world& aWorld)
{
    aWorld.component<glm::vec3>()
        .member("x", &glm::vec3::x)
        .member("y", &glm::vec3::y)
        .member("z", &glm::vec3::z);

    aWorld.component<MovementComponent>()
        .member("Position", &MovementComponent::Position)
        .member("Rotation", &MovementComponent::Rotation)
        .member("Tick", &MovementComponent::Tick)
        .member("Speed", &MovementComponent::Velocity);

    auto observer = aWorld.observer<const MovementComponent>("Movement Replication")
        .event(flecs::OnSet)
        // .without<AttachmentComponent>()
        .each(
            [](flecs::iter& it, size_t i, const MovementComponent& aComponent)
            {
                const auto entity = it.entity(i);

                ReplicateMovementComponent(entity, aComponent);
            });

    observer.child_of(aWorld.entity("observers"));
}
