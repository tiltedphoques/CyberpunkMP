#pragma once

struct MovementComponent
{
    glm::vec3 Position;
    glm::vec3 Rotation;
    float Velocity;
    uint64_t Tick;

    static void Register(flecs::world& aWorld);
};