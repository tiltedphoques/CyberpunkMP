#pragma once

#include "RED4ext/Scripting/Natives/Generated/anim/AnimFeature_CrowdLocomotion.hpp"
#include "RED4ext/Scripting/Natives/Generated/anim/AnimFeature_Locomotion.hpp"
#include "RED4ext/Scripting/Natives/Generated/ent/AnimationControllerComponent.hpp"

struct AnimationData;

struct AnimationDriver
{
    ~AnimationDriver();

    void Attach(Red::Entity* parent);
    void Detach();

    void SendParameters(AnimationData& data);

    float GetAnimLength(Red::CName name) const;

    Red::Handle<Red::anim::AnimFeature_Locomotion> locomotion;
    Red::Handle<Red::anim::AnimFeature_CrowdLocomotion> crowd;
    Red::WeakHandle<Red::ent::AnimationControllerComponent> component;
};
