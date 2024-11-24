#pragma once

struct VehicleComponent
{
    uint64_t TweakDBID{0};

    static void Register(flecs::world& aWorld);
};