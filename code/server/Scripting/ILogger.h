#pragma once

#include "ScriptingBase.h"

struct ILogger
{
    virtual void Debug(const char* aMessage) = 0;
    virtual void Info(const char* aMessage) = 0;
    virtual void Warn(const char* aMessage) = 0;
    virtual void Error(const char* aMessage) = 0;

    TP_EXPORT static ILogger* Get(const char* aName);
};