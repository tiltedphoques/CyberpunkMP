#include "SteamInterface.h"
#include "steam/steamnetworkingsockets.h"

#include <atomic>


static std::atomic<std::size_t> s_initCounter = 0;

void SteamInterface::Acquire()
{
    if (s_initCounter.fetch_add(1, std::memory_order_relaxed) == 0)
    {
        SteamDatagramErrMsg errorMessage;
        if (!GameNetworkingSockets_Init(nullptr, errorMessage))
        {
            // TODO: Error management
        }
    }
}

void SteamInterface::Release()
{
    if (s_initCounter.fetch_sub(1, std::memory_order_relaxed) == 1)
    {
        GameNetworkingSockets_Kill();
    }
}

