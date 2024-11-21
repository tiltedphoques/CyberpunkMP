// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <optional>
#include <string>

namespace oobe
{
struct LoginData
{
    std::string Username;
    std::string Token;
};

std::optional<LoginData> StartLogin();
}
