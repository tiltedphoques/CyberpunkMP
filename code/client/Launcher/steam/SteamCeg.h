#pragma once

#include "SteamCrypto.h"
#include <mem/mem.h>

namespace steam
{
uintptr_t CrackCEGInPlace(uint8_t* start, mem::region textSeg);
} // namespace steam
