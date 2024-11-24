#pragma once

#include "IPlayerManager.h"

struct PlayerScriptInstance;
struct PlayerManager;


using TPlayerCallback = void (*)(uint64_t);
struct PlayerManagerScriptInstance final : IPlayerManager
{
    PlayerManagerScriptInstance(PlayerManager* apPlayerManager);
    ~PlayerManagerScriptInstance() override;
    IPlayer* GetPlayer(uint64_t Id) override;

protected:

    void PlayerJoin(uint64_t aId) const;
    void PlayerLeft(uint64_t aId) const;

private:
    PlayerManager* m_pPlayerManager{nullptr};
    Map<uint64_t, UniquePtr<PlayerScriptInstance>> m_scriptInstances;
    TPlayerCallback m_joinCallback{nullptr};
    TPlayerCallback m_leftCallback{nullptr};
};
