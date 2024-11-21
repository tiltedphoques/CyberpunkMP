#include "PlayerManagerScriptInstance.h"
#include "PlayerScriptInstance.h"
#include "Components/PlayerComponent.h"

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


void PlayerManagerScriptInstance::PlayerJoin(uint64_t aId) const
{
    if (m_joinCallback != nullptr)
    {
        m_joinCallback(&aId, sizeof(aId));
    }
}

void PlayerManagerScriptInstance::PlayerLeft(uint64_t aId) const
{
    if (m_leftCallback != nullptr)
    {
        m_leftCallback(&aId, sizeof(aId));
    }
}

PlayerManagerScriptInstance::PlayerManagerScriptInstance(PlayerManager* apPlayerManager)
    : m_pPlayerManager(apPlayerManager)
{
    m_joinCallback = GServer->GetScripting()->GetFunction(L"PlayerJoin");
    if (!m_joinCallback)
        spdlog::error("Unable to find PlayerJoin function in Sdk, server will crash!!!");

    m_leftCallback = GServer->GetScripting()->GetFunction(L"PlayerLeft");
    if (!m_leftCallback)
        spdlog::error("Unable to find PlayerLeft function in Sdk, server will crash!!!");

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
