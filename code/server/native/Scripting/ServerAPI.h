#pragma once

#include "ScriptingBase.h"

using TUpdateCallback = void(*)(float);
using TPlayerEvent = void(*)(uint64_t);

struct ServerAPI
{
    TP_EXPORT static void Initialize();
    TP_EXPORT static void Run();
    TP_EXPORT static void Exit();
    TP_EXPORT static void SetUpdateCallback(TUpdateCallback callback);
    TP_EXPORT static void SetPlayerJoinCallback(TPlayerEvent callback);
    TP_EXPORT static void SetPlayerLeftCallback(TPlayerEvent callback);
};