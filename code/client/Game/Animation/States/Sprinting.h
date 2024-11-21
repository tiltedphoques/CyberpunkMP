#pragma once

#include "Base.h"

namespace States
{
struct Sprinting : Base
{
    Sprinting(MultiMovementController& aParent)
        : Base(aParent)
    {
    }

    ~Sprinting() override{}
    void Enter() noexcept override;

    void GetAnimationData(AnimationData& aData) const override;

    std::optional<Transition> Process(const Update& acEvent) noexcept override;

private:

    float m_timer = 0.f;
    float m_duration = 0.f;
};
} // namespace States
