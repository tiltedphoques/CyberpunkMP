#include "Components/MovementComponent.h"
#include "MovementComponentScriptInstance.h"
#include <Components/PlayerComponent.h>


MovementComponentScriptInstance::MovementComponentScriptInstance(flecs::entity aPuppet)
    : m_puppet(aPuppet)
{

}

MovementComponentScriptInstance::~MovementComponentScriptInstance()
{
}

Vector3 MovementComponentScriptInstance::GetPosition()
{
    const auto pComponent = m_puppet.get<PlayerComponent>();
    if (pComponent)
    {
        auto pMovementComponent = pComponent->Puppet.get<MovementComponent>();
        if (pMovementComponent)
        {
            auto vec = pMovementComponent->Position;
            return Vector3{vec.x, vec.y, vec.z};
        }
    }
    return Vector3{0, 0, 0};
}

Vector3 MovementComponentScriptInstance::GetRotation()
{
    const auto pComponent = m_puppet.get<PlayerComponent>();
    if (pComponent)
    {
        auto pMovementComponent = pComponent->Puppet.get<MovementComponent>();
        if (pMovementComponent)
        {
            auto vec = pMovementComponent->Rotation;
            return Vector3{vec.x, vec.y, vec.z};
        }
    }
    return Vector3{0, 0, 0};
}

float MovementComponentScriptInstance::GetVelocity()
{
    const auto pComponent = m_puppet.get<PlayerComponent>();
    if (pComponent)
    {
        auto pMovementComponent = pComponent->Puppet.get<MovementComponent>();
        if (pMovementComponent)
        {
            auto vec = pMovementComponent->Velocity;
            return vec;
        }
    }
    return 0.f;
}
