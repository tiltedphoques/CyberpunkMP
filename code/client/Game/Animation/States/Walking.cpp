#include "Walking.h"

#include "Idling.h"
#include "Sprinting.h"

#include "Game/Animation/AnimationData.h"
#include "Game/Animation/MultiMovementController.h"

namespace States
{ 
void Walking::Enter() noexcept
{
    m_timer = 0.f;
    m_duration = m_parent.GetAnimLength("walk_0");
}

void Walking::GetAnimationData(AnimationData& aData) const
{
    aData.action = MTA_Move;
    aData.style = LS_Walk;
    aData.time = m_timer;
}

std::optional<Base::Transition> Walking::Process(const Update& acEvent) noexcept
{
    if (acEvent.Speed >= kRunSpeed)
        return Transit<Sprinting>();
    if (acEvent.Speed < kWalkSpeed)
        return Transit<Idling>();

    m_timer += acEvent.Delta;
    m_timer = std::fmodf(m_timer, m_duration);

    return std::nullopt;
}
}
