
/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#include <cstdint>

// Changes:
// - 2021/2/23: use TLS touched class to automatically initialize TLS.
// - 2021/2/24: Lowered necessary TLS pool size for Fallout4 and SkyrimSE.
// - 2023/2/16: Lowered TLS for UE games

/* #pragma optimize("", off)
#pragma init_seg(lib)

#define DECLARE_TLS_VARS(i) static thread_local uint8_t tls[sizeof(int) * i]

DECLARE_TLS_VARS(1000000);

#include <Windows.h>

struct TlsToucher
{
    TlsToucher() { *reinterpret_cast<int*>(&tls[400000]) = static_cast<int>(GetTickCount64()); }
};

TlsToucher toucher;

#pragma optimize("", on)*/
