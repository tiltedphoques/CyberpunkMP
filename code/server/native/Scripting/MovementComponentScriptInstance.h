#pragma once

#include "IMovementComponent.h"

struct MovementComponentScriptInstance final : IMovementComponent
{
    MovementComponentScriptInstance(flecs::entity aPuppet);
    ~MovementComponentScriptInstance() override;

    // Inherited via IMovementComponent
    Vector3 GetPosition() override;
    Vector3 GetRotation() override;
    float GetVelocity() override;

protected:
    flecs::entity m_puppet;
};
