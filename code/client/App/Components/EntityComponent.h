#pragma once
#include "Game/Animation/MultiMovementController.h"

struct EntityComponent
{
    Red::EntityID Id;
    bool IsVehicle{false};
    MultiMovementController* Controller{nullptr};
};
