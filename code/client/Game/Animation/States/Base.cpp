#include "Base.h"

#include "Game/Animation/AnimationData.h"

namespace States
{
void Base::GetAnimationData(AnimationData& aData) const
{
    aData.action = MTA_None;
    aData.style = LS_Idle;
}

std::optional<Base::Transition> Base::Process(const Update& acEvent) noexcept
{
    return std::nullopt;
}
}
