#pragma once

#include "Base.h"

namespace States
{
struct Idling : Base
{
    Idling(MultiMovementController& aParent)
        : Base(aParent)
    {
    }

    ~Idling() override{}
    void Enter() noexcept override;

    std::optional<Transition> Process(const Update& acEvent) noexcept override;
};
} // namespace States
