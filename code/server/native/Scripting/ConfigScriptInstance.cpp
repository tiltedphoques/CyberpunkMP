#include "IConfig.h"

#include <GameServer.h>

TP_EXPORT const IConfig* IConfig::Get()
{
    return GServer->GetConfig();
}

