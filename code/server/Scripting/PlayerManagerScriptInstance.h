#pragma once

#include "IPlayerManager.h"

struct PlayerScriptInstance;
struct PlayerManager;
struct PlayerManagerScriptInstance final : IPlayerManager
{
    PlayerManagerScriptInstance(PlayerManager* apPlayerManager);
    ~PlayerManagerScriptInstance() override;
    IPlayer* GetPlayer(uint64_t Id) override;

protected:

    void PlayerJoin(uint64_t aId) const;
    void PlayerLeft(uint64_t aId) const;

private:
    PlayerManager* m_pPlayerManager;
    Map<uint64_t, UniquePtr<PlayerScriptInstance>> m_scriptInstances;
    component_entry_point_fn m_joinCallback;
    component_entry_point_fn m_leftCallback;
};
