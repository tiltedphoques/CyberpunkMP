#pragma once

struct AttachmentComponent
{
    flecs::entity Parent;
    uint64_t SlotId;

    static void Register(flecs::world& aWorld);
};