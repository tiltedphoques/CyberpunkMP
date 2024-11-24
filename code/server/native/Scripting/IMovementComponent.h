#pragma once

#include "ScriptingBase.h"

struct Vector3
{
    float X;
    float Y;
    float Z;
};

struct IMovementComponent
{
    IMovementComponent(const IMovementComponent&) = delete;
    virtual ~IMovementComponent() {}

    virtual Vector3 GetPosition() = 0;
    virtual Vector3 GetRotation() = 0;
    virtual float GetVelocity() = 0;

protected:

    IMovementComponent() = default;
};
