#pragma once
#include "Core/Stl.h"

struct Path;

struct Log
{
    Log(Path& aPath);
    ~Log();

    std::shared_ptr<spdlog::logger> Create(
        const String& aName, spdlog::sink_ptr aExtraSink = nullptr, const String& aPattern = "[%Y-%m-%d %H:%M:%S UTC%z] [%^%l%$] [%n] %v");

private:
    std::filesystem::path m_logRoot;
};
