// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "AuthenticationProcess.h"
#include "Launcher/utils/Platform.h"

namespace oobe
{
std::optional<LoginData> StartLogin()
{
    return LoginData{"Test", "Token"};
}
}
