#pragma once

#include "Scripting/WorldScriptInstance.h"

struct World : flecs::world
{
    TP_NOCOPYMOVE(World);

    World();
    ~World();

    void Update(float aDelta);

    gsl::not_null<WorldScriptInstance*> GetScriptInstance() noexcept;
    gsl::not_null<const WorldScriptInstance*> GetScriptInstance() const noexcept;

private:

    WorldScriptInstance m_scriptInstance;
};
