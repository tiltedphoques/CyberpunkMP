#include "AnimationDriver.h"


AnimationDriver::~AnimationDriver()
{
    Detach();
}

void AnimationDriver::Attach(Red::Entity* parent)
{
    static Core::RawFunc<2624656810UL, decltype(&AnimationDriver::Attach)> attach;
    attach(this, parent);
}

void AnimationDriver::Detach()
{
    static Core::RawFunc<2385973466UL, decltype(&AnimationDriver::Detach)> detach;
    detach(this);
}

void AnimationDriver::SendParameters(AnimationData& data)
{
    static Core::RawFunc<2132808949UL, decltype(&AnimationDriver::SendParameters)> send;
    send(this, data);
}

float AnimationDriver::GetAnimLength(Red::CName name) const
{
    static Core::RawFunc<3750304414UL, float (*)(Red::AnimationControllerComponent*, Red::CName)> get;
    if (const auto pComp = component.Lock())
        return get(pComp.GetPtr(), name);
    return 0.f;
}
