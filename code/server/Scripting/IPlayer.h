#pragma once

#include "ScriptingBase.h"

struct IBuffer;
struct IMovementComponent;
struct IAttachmentComponent;

struct IPlayer
{
    IPlayer(const IPlayer&) = delete;

    virtual ~IPlayer(){}
    virtual uint64_t GetId() const = 0;
    virtual uint64_t GetPuppetId() const = 0;
    virtual uint32_t GetConnectionId() const = 0;
    virtual IMovementComponent* GetMovementComponent() = 0;
    virtual IAttachmentComponent* GetAttachmentComponent() = 0;
    virtual const char* GetUsername() const = 0;
    virtual void SendChat(const char* From, const char* Message) = 0;
    virtual void Call(uint64_t Klass, uint64_t Function, IBuffer* Arguments) = 0;

protected:

    IPlayer() = default;
};
