#pragma once

#include "Base.h"

namespace States
{
struct Walking : Base
{
    Walking(MultiMovementController& aParent)
        : Base(aParent)
    {
    }

    ~Walking() override {}
    void Enter() noexcept override;
    void GetAnimationData(AnimationData& aData) const override;

    std::optional<Transition> Process(const Update& acEvent) noexcept override;

private:

    float m_timer = 0.f;
    float m_duration = 0.f;
};
} // namespace States
