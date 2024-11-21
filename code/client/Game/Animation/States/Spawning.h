#pragma once

#include "Base.h"

namespace States
{
struct Spawning : Base
{
    Spawning(MultiMovementController& aParent)
        : Base(aParent)
    {
    }

    ~Spawning() override{}
    void Enter() noexcept override;

    std::optional<Transition> Process(const Update& acEvent) noexcept override;
};
} // namespace States
