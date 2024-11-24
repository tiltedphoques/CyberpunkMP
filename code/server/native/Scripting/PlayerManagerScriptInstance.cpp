#include "PlayerManagerScriptInstance.h"
#include "PlayerScriptInstance.h"
#include "Components/PlayerComponent.h"

#include "PlayerManager.h"

#include "GameServer.h"

TP_EXPORT IPlayerManager* IPlayerManager::Get()
{
    return GServer->GetWorld()->get_mut<PlayerManager>()->GetScriptInstance();
}

IPlayer* PlayerManagerScriptInstance::GetPlayer(uint64_t Id)
{
    const auto it = m_scriptInstances.find(Id);
    if (it == std::end(m_scriptInstances))
        return nullptr;

    return it.value().get();
}

void PlayerManagerScriptInstance::SetPlayerJoinCallback(TPlayerCallback callback)
{
    m_joinCallback = callback;
}

void PlayerManagerScriptInstance::SetPlayerLeftCallback(TPlayerCallback callback)
{
    m_leftCallback = callback;
}


void PlayerManagerScriptInstance::PlayerJoin(uint64_t aId) const
{
    if (m_joinCallback != nullptr)
    {
        m_joinCallback(aId);
    }
}

void PlayerManagerScriptInstance::PlayerLeft(uint64_t aId) const
{
    if (m_leftCallback != nullptr)
    {
        m_leftCallback(aId);
    }
}

PlayerManagerScriptInstance::PlayerManagerScriptInstance(PlayerManager* apPlayerManager)
    : m_pPlayerManager(apPlayerManager)
{
    auto observer = m_pPlayerManager->GetWorld()->observer<PlayerComponent>("Player Manager Script Observer")
        .event(flecs::OnSet)
        .event(flecs::OnRemove)
        .each([this](flecs::iter& it, size_t i, PlayerComponent& component)
        {
            const auto id = it.entity(i).raw_id();
            if (it.event() == flecs::OnSet)
            {
                m_scriptInstances[id] = MakeUnique<PlayerScriptInstance>(it.entity(i));
                PlayerJoin(id);
            }
            else
            {
                PlayerLeft(id);
                m_scriptInstances.erase(id);
            }
        });
    observer.child_of(m_pPlayerManager->GetWorld()->entity("observers"));
}

PlayerManagerScriptInstance::~PlayerManagerScriptInstance()
{
}
