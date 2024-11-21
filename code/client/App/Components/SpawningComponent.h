#pragma once

struct MultiMovementController;
struct SpawningComponent
{
    Red::EntityID Id;
    MultiMovementController* Controller{nullptr};
};