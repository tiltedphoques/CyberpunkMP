#include "AttachmentComponent.h"
#include "GameServer.h"
#include "PlayerComponent.h"

void ReplicateSetAttachmentComponent(flecs::entity aEntity, AttachmentComponent& aComponent)
{
    server::NotifyVehicleEnter enter;
    enter.set_vehicle_id(aComponent.Parent);
    enter.set_character_id(aEntity);
    enter.set_sit_id(aComponent.SlotId);

    spdlog::info("AttachmentComponent::ReplicateSetAttachmentComponent: Slot: {}", aComponent.SlotId);

    aEntity.world().each(
        [aEntity, &enter](flecs::entity player, const PlayerComponent& aPlayerComponent)
        {
            if (!IsDebug() && player == aEntity.parent())
                return;

            GServer->Send(aPlayerComponent.Connection, enter);
        });

    if (aComponent.SlotId == 0xb000b1d029d0cea0ULL) // seat_front_left
    {
        server::NotifyVehicleControlAssigned control;
        control.set_vehicle_id(aComponent.Parent);

        aComponent.Parent.child_of(aEntity.parent());

        spdlog::info("Assign control of vehicle {:x}", control.get_vehicle_id());

        if (auto* pPlayerComponent = aEntity.parent().get<PlayerComponent>())
            GServer->Send(pPlayerComponent->Connection, control);
    }
}

void ReplicateRemoveAttachmentComponent(flecs::entity aEntity, const AttachmentComponent& aComponent)
{
    server::NotifyVehicleExit message;
    message.set_character_id(aEntity);

    spdlog::info("AttachmentComponent::ReplicateRemoveAttachmentComponent");

    aEntity.world().each(
        [aEntity, &message](flecs::entity player, const PlayerComponent& aPlayerComponent)
        {
            if (!IsDebug() && player == aEntity.parent())
                return;

            GServer->Send(aPlayerComponent.Connection, message);
        });
}

void AttachmentComponent::Register(flecs::world& aWorld)
{
    aWorld.component<AttachmentComponent>()
        .member("Parent", &AttachmentComponent::Parent)
        .member("Slot", &AttachmentComponent::SlotId);

    auto observer = aWorld.observer<AttachmentComponent>("Attachment set replication")
        .event(flecs::OnSet)
        .each(
            [](flecs::iter& it, size_t i, AttachmentComponent& aComponent)
            {
                const auto entity = it.entity(i);

                ReplicateSetAttachmentComponent(entity, aComponent);
            });

    observer.child_of(aWorld.entity("observers"));

    observer = aWorld.observer<const AttachmentComponent>("Attachment remove replication")
       .event(flecs::OnRemove)
       .each(
           [](flecs::iter& it, size_t i, const AttachmentComponent& aComponent)
           {
               const auto entity = it.entity(i);

               ReplicateRemoveAttachmentComponent(entity, aComponent);
           });

    observer.child_of(aWorld.entity("observers"));
}
