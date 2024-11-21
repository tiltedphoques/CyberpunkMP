#pragma once

#include "Loader/ExeLoader.h"
#include <Core/Filesystem.h>
#include <Core/Stl.h>

namespace launcher
{
namespace fs = std::filesystem;

enum class Result
{
    kSuccess,
    kBadPlatform,
    kBadInstall
};

// stays alive through the entire duration of the game.
struct LaunchContext
{
    fs::path exePath;
    fs::path gamePath;
    String Version;
    ExeLoader::TEntryPoint gameMain = nullptr;
    String ip = "127.0.0.1";
    uint16_t port = 11778;
    Vector<fs::path> mods = {};
    bool enabled = false;
};

LaunchContext* GetLaunchContext();

bool LoadProgram(LaunchContext&);
int StartUp(int argc, char** argv);
} // namespace launcher
