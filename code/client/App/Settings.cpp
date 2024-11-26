#include "Settings.h"
#include <RED4ext/LaunchParameters.hpp>

void Settings::Load()
{
    Settings& settings = Get();

    auto& launchParameters = RED4ext::GetLaunchParameters();

    if (launchParameters.Contains(RED4ext::CString("-online")))
        settings.enabled = true;

    if (const auto ip = launchParameters.Get("-ip"); ip)
    {
        if (ip->size > 0)
            settings.ip = (*ip)[0].c_str();
    }

    if (const auto port = launchParameters.Get("-port"); port)
    {
        if (port->size > 0)
            settings.ip = (*port)[0].c_str();
    }

    if (const auto mods = launchParameters.Get("-mod"); mods)
    {
        for (auto mod : *mods)
            settings.mods.push_back(mod.c_str());
    }
}
