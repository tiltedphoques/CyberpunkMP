#pragma once

#include "ScriptingBase.h"

struct IPlayer;
CS_INTERNAL struct IPlayerManager
{
    using PlayerIdHandler = void (*)(uint64_t Id);

    virtual ~IPlayerManager(){}
    virtual IPlayer* GetPlayer(uint64_t Id) = 0;

    TP_EXPORT static IPlayerManager* Get();
};