#pragma once

#include "IWorld.h"

using TUpdateCallback = void (*)(float);

struct WorldScriptInstance : IWorld
{
    WorldScriptInstance();
    virtual ~WorldScriptInstance();

    void Initialize();

    void SetUpdateCallback(TUpdateCallback apCallback);

private:

    flecs::system m_updateSystem;
    TUpdateCallback m_callback{nullptr};
};
