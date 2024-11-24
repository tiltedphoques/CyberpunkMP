#pragma once

struct InterpolationComponent
{
    struct Timepoint
    {
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Velocity;
        uint64_t Tick;
    };

    List<Timepoint> TimePoints;
};