#pragma once

#include "Scripting/IConfig.h"

struct Config : IConfig
{
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, Name, Description, IconUrl, MaxPlayer, Tags, TickRate, UpdateRate, Public, Port, Password, ApiKey)
};