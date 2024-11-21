#pragma once

#include "IPlayer.h"
#include "MovementComponentScriptInstance.h"
#include "AttachmentComponentScriptInstance.h"

struct PlayerScriptInstance final : IPlayer
{
    PlayerScriptInstance(flecs::entity aPlayer);
    ~PlayerScriptInstance() override;

    uint64_t GetId() const override;
    uint32_t GetConnectionId() const override;
    uint64_t GetPuppetId() const override;
    IMovementComponent* GetMovementComponent() override;
    IAttachmentComponent* GetAttachmentComponent() override;
    const char* GetUsername() const override;
    void SendChat(const char* From, const char* Message) override;
    void Call(uint64_t Klass, uint64_t Function, IBuffer* Arguments) override;

protected:
    flecs::entity m_player;
    MovementComponentScriptInstance m_movementComponent;
    AttachmentComponentScriptInstance m_attachmentComponent;
};
