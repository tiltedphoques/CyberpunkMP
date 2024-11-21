#pragma once

#include "ScriptingBase.h"

struct IConfig
{
    std::string Name{"Default"};
    std::string Description{""};
    std::string ApiKey{""};
    std::string IconUrl{""};
    uint16_t MaxPlayer{4};
    std::string Tags{""};
    bool Public{false};
    uint16_t Port{11778};
    uint16_t WebPort{11779};
    uint16_t TickRate{60};
    uint16_t UpdateRate{10};
    std::string Password{""};

    TP_EXPORT static const IConfig* Get();
};