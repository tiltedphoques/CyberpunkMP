#pragma once

#include "ScriptingBase.h"

using TUpdateCallback = void(*)(float);
struct ServerAPI
{
    TP_EXPORT static void Initialize();
    TP_EXPORT static void Run();
    TP_EXPORT static void Exit();
    TP_EXPORT static void SetUpdateCallback(TUpdateCallback callback);
};