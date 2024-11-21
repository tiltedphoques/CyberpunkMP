#pragma once

#include "steam/steamnetworkingsockets.h"


enum EPacketFlags
{
    kReliable,
    kUnreliable
};

enum EConnectOpcode : uint8_t
{
    kPayload = 0,
    kServerTime = 1,
    kCompressedPayload = 2,
    kHandshake = 3
};

struct SteamInterface
{
    static void Acquire();
    static void Release();
};

using ConnectionId = HSteamNetConnection;

