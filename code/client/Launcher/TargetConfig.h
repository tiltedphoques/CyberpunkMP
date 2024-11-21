// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <cstdint>
#include <limits>

#define CLIENT_DLL 0

struct TargetConfig
{
    const wchar_t* dllClientName;
    const wchar_t* fullGameName;
    uint32_t steamAppId;
    uint32_t exeLoadSz;
    // Needs to be kept up to date.
    uint32_t exeDiskSz;
};

// clang-format off
static constexpr TargetConfig CurrentTarget{
    L"Verycyber.dll",
    L"Cyberpunk",
    1091500, 0x6000000, 59076608  };
#define TARGET_NAME L"VeryCyber"
#define PRODUCT_NAME L"VeryCyber"
#define SHORT_NAME L"VeryCyber"

// clang-format on
