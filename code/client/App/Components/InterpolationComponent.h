#pragma once

struct InterpolationComponent
{
    struct Timepoint
    {
        glm::vec3 Position{};
        glm::vec3 Rotation{};
        float Velocity{0.f};
        uint64_t Tick{0};
    };

    List<Timepoint> TimePoints{};
    Timepoint PreviousFrame{};
};