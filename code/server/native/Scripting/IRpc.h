#pragma once

#include "ScriptingBase.h"

struct IPlayer;
struct IBuffer;

CS_INTERNAL struct RpcCall
{
    uint32_t Id;
    IBuffer* Args;
    uint64_t PlayerId;
};

CS_INTERNAL struct IRpc
{
    virtual void Call(uint64_t PlayerId, uint64_t Klass, uint64_t Function, IBuffer* Arguments) = 0;

    TP_EXPORT static IRpc* Get();

    CS_INTERNAL virtual uint32_t Count() = 0;
    CS_INTERNAL virtual RpcCall* GetRpc(uint32_t aIndex) = 0;
    CS_INTERNAL virtual void Clear() = 0;
    CS_INTERNAL virtual uint32_t RegisterServer(uint64_t Klass, uint64_t Function) = 0;
    CS_INTERNAL virtual uint32_t RegisterClient(uint64_t Klass, uint64_t Function) = 0;
};