#pragma once

#include <coreclr_delegates.h>

#include "IWorld.h"

struct WorldScriptInstance : IWorld
{
    WorldScriptInstance();
    virtual ~WorldScriptInstance();

    void Initialize();

private:

    flecs::system m_updateSystem;
    component_entry_point_fn m_callback;
};
