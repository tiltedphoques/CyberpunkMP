#pragma once

#include "Config.h"
#include "Scripting/WorldScriptInstance.h"

struct World : flecs::world
{
    TP_NOCOPYMOVE(World);

    World(const FlecsConfig& acFlecsConfig);
    ~World();

    void Update(float aDelta);

    gsl::not_null<WorldScriptInstance*> GetScriptInstance() noexcept;
    gsl::not_null<const WorldScriptInstance*> GetScriptInstance() const noexcept;

private:

    WorldScriptInstance m_scriptInstance;
};
