
#include "TargetConfig.h"
#include "launcher.h"

#include "loader/ExeLoader.h"
#include "loader/PathRerouting.h"

#include "Utils/Error.h"
#include "Utils/FileVersion.inl"

#include "oobe/PathSelection.h"
#include "oobe/AuthenticationProcess.h"
#include "steam/SteamLoader.h"

extern void LaterInstall();
extern void ApplyStartupHook();

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
HICON g_SharedWindowIcon = nullptr;

namespace launcher
{
oobe::LoginData g_loginData;

LaunchContext* GetLaunchContext()
{
    static LaunchContext g_context;
    return &g_context;
}

// Everything is nothing, life is worth living, just look to the stars
#define DIE_NOW(err)  \
    {                 \
        Die(err);     \
        return false; \
    }

bool LoadMods(const std::filesystem::path& acExe)
{
    auto rootPath = acExe.parent_path().parent_path();

    if (::LoadLibraryW((rootPath / L"red4ext" / L"RED4ext.dll").c_str()) == nullptr)
        DIE_NOW(L"Failed to load RED4ext.dll. Please install it and try again.");

    const auto asiRoot = acExe / "plugins";
    if (exists(asiRoot))
    {
        for (auto const& entry : std::filesystem::directory_iterator{asiRoot, std::filesystem::directory_options::follow_directory_symlink})
        {
            std::error_code ec;
            if (entry.is_regular_file(ec))
            {
                auto path = entry.path();
                if (path.extension() == ".asi")
                    ::LoadLibraryW(path.c_str());
            }
        }
    }

    return true;
}

int StartUp(int argc, char** argv)
{
    bool askSelect = (::GetAsyncKeyState(VK_SPACE) & 0x8000);
    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "--ip") == 0)
        {
            i++;
            if (i < argc)
                GetLaunchContext()->ip = argv[i];
        }
        else if (std::strcmp(argv[i], "--port") == 0)
        {
            i++;
            if (i < argc)
                GetLaunchContext()->port = std::stoul(argv[i]) & 0xFFFF;
        }
        else if (std::strcmp(argv[i], "--mod") == 0)
        {
            i++;
            if (i < argc)
                GetLaunchContext()->mods.emplace_back(argv[i]);
        }
        else if (std::strcmp(argv[i], "--online") == 0)
        {
            GetLaunchContext()->enabled = true;
        }
    }

    LaterInstall();
    ApplyStartupHook(); // this gives us a control flow on the games main thread for the user code init.

    return 0;
}

bool LoadProgram(LaunchContext& LC)
{
    auto content = LoadFile(LC.exePath);
    if (content.empty())
        DIE_NOW(L"Failed to mount game executable");

    LC.Version = ::QueryFileVersion(LC.exePath.c_str());
    // if (LC.Version.empty())
    //     DIE_NOW(L"Failed to query game version");

    ExeLoader loader(CurrentTarget.exeLoadSz);
    if (!loader.Load(reinterpret_cast<uint8_t*>(content.data())))
        DIE_NOW(L"Fatal error while mapping executable");

    LC.gameMain = loader.GetEntryPoint();
    return true;
}
} // namespace launcher
