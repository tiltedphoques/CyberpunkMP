#pragma once

#include "ScriptingBase.h"

struct IConfig
{
    virtual const char* GetName() const = 0;
    virtual const char* GetDescription() const = 0;
    virtual const char* GetApiKey() const = 0;
    virtual const char* GetIconUrl() const = 0;
    virtual const char* GetTags() const = 0;
    virtual const char* GetPassword() const = 0;
    virtual uint16_t GetMaxPlayer() const = 0;
    virtual uint16_t GetPort() const = 0;
    virtual uint16_t GetWebPort() const = 0;
    virtual uint16_t GetTickRate() const = 0;
    virtual uint16_t GetUpdateRate() const = 0;
    virtual bool GetPublic() const = 0;

    TP_EXPORT static const IConfig* Get();
};