#include "PlayerScriptInstance.h"
#include "Components/PlayerComponent.h"
#include <GameServer.h>
#include "IRpc.h"


PlayerScriptInstance::PlayerScriptInstance(flecs::entity aPlayer)
    : m_player(aPlayer)
    , m_movementComponent(aPlayer)
    , m_attachmentComponent(aPlayer)
{
}

PlayerScriptInstance::~PlayerScriptInstance()
{
}

uint64_t PlayerScriptInstance::GetId() const
{
    return m_player.raw_id();
}

uint32_t PlayerScriptInstance::GetConnectionId() const
{
    if (const auto pComponent = m_player.get<PlayerComponent>())
        return pComponent->Connection;

    return -1;
}

uint64_t PlayerScriptInstance::GetPuppetId() const
{
    if (const auto pComponent = m_player.get<PlayerComponent>())
        return pComponent->Puppet.raw_id();

    return -1;
}

IMovementComponent* PlayerScriptInstance::GetMovementComponent()
{
    return &m_movementComponent;
}

IAttachmentComponent* PlayerScriptInstance::GetAttachmentComponent()
{
    return &m_attachmentComponent;
}

const char* PlayerScriptInstance::GetUsername() const
{
    if (const auto pComponent = m_player.get<PlayerComponent>())
        return pComponent->Username.c_str();

    return "None";
}

void PlayerScriptInstance::SendChat(const char* From, const char* Message)
{
    if (const auto pComponent = m_player.get<PlayerComponent>())
    {
        server::ChatMessage msg;
        msg.set_username(From);
        msg.set_message(Message);
        GServer->Send(pComponent->Connection, msg);
    }
    else
        spdlog::get("Scripting")->warn("Attempt to send a message to a player that doesn't have a PlayerComponent!");
}

void PlayerScriptInstance::Call(uint64_t Klass, uint64_t Function, IBuffer* Arguments)
{
    IRpc::Get()->Call(GetId(), Klass, Function, Arguments);
}
