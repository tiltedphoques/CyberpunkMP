#include "Launcher/launcher.h"
#include "Launcher/utils/ComUtils.h"
#include "Launcher/oobe/AuthenticationProcess.h"

#include <shellapi.h>
#include <ArchiveXL/support/red4ext/ArchiveXL.hpp>
#include <TweakXL/support/red4ext/TweakXL.hpp>

#include "RED4ext/Api/EMainReason.hpp"

extern void CoreStubsInit();

std::filesystem::path GCyberpunkMpLocation;

namespace launcher
{
extern oobe::LoginData g_loginData;
} // namespace launcher

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

int FakeMain(int argc, char** argv)
{
    // get the exe location before module hooks are installed
    namespace fs = std::filesystem;

    constexpr auto pathLength = MAX_PATH + 1;
    std::string filename;
    do
    {
        filename.resize(filename.size() + pathLength, '\0');

        const auto length = GetModuleFileName((HINSTANCE)&__ImageBase, filename.data(), static_cast<uint32_t>(filename.size()));
        if (length > 0)
        {
            filename.resize(length);
        }
    } while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

    fs::path path = filename;
    if (is_symlink(path))
    {
        path = read_symlink(path);
    }

    GCyberpunkMpLocation = path.parent_path();

#ifndef DEBUG

    const auto result = oobe::StartLogin();
    if (!result)
        return 0;

    launcher::g_loginData = result.value();

#endif

    return launcher::StartUp(argc, argv);
}

void GetCommandLineArgs(int* argc, char*** argv)
{
    // Get the command line arguments as wchar_t strings
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), argc);
    if (!wargv)
    {
        *argc = 0;
        *argv = NULL;
        return;
    }

    // Count the number of bytes necessary to store the UTF-8 versions of those strings
    int n = 0;
    for (int i = 0; i < *argc; i++)
        n += WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL) + 1;

    // Allocate the argv[] array + all the UTF-8 strings
    *argv = (char**)malloc((*argc + 1) * sizeof(char*) + n);
    if (!*argv)
    {
        *argc = 0;
        return;
    }

    // Convert all wargv[] --> argv[]
    char* arg = (char*)&((*argv)[*argc + 1]);
    for (int i = 0; i < *argc; i++)
    {
        (*argv)[i] = arg;
        arg += WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, arg, n, NULL, NULL) + 1;
    }
    (*argv)[*argc] = NULL;
}

#include "App/Application.h"

RED4EXT_C_EXPORT bool Main(RED4ext::PluginHandle aHandle, RED4ext::EMainReason aReason, const RED4ext::Sdk* aSdk)
{

    switch (aReason)
    {
    case RED4ext::EMainReason::Load:
    {
        int argc;
        char** argv;
        GetCommandLineArgs(&argc, &argv);

        FakeMain(argc, argv);

        free(argv);

        if (!launcher::GetLaunchContext()->enabled)
            return false;

        App::GApplication = MakeUnique<App::Application>(aHandle, aSdk);
        App::GApplication->Bootstrap();

        const auto scriptPath = GCyberpunkMpLocation / TP_REDSCRIPT_LOCATION;
        const wchar_t* scriptLocation = canonical(scriptPath).c_str();
        aSdk->scripts->Add(aHandle, scriptLocation);

        ArchiveXL::RegisterArchives(canonical(GCyberpunkMpLocation / TP_ARCHIVES_LOCATION));
        TweakXL::RegisterTweaks(canonical(GCyberpunkMpLocation / TP_TWEAKS_LOCATION));

        const auto inputLoaderModule = LoadLibrary("input_loader");
        void(*pInputLoaderAdd)(RED4ext::PluginHandle, const wchar_t*);
        if (inputLoaderModule != nullptr && (pInputLoaderAdd = reinterpret_cast<decltype(pInputLoaderAdd)>(GetProcAddress(inputLoaderModule, "Add"))))
        {
            const wchar_t* input_path = canonical(GCyberpunkMpLocation / TP_INPUTS_LOCATION).c_str();
            pInputLoaderAdd(aHandle, input_path);
        }
        else
        {
            const auto message = 
                L"The following CyperpunkMP requirements were not met:\n\n* Input Loader v0.2.0\nPlease ensure the mods "
                L"above are installed/up-to-date.";
            MessageBoxW(nullptr, message, L"CyperpunkMP requirements could not be found", MB_SYSTEMMODAL | MB_ICONERROR);
            return false;
        }

        for (auto& mod : launcher::GetLaunchContext()->mods)
        {
            aSdk->scripts->Add(aHandle, mod.wstring().c_str());
            ArchiveXL::RegisterArchives(mod);
            TweakXL::RegisterTweaks(mod);
            pInputLoaderAdd(aHandle, mod.wstring().c_str());
        }

        break;
    }
    case RED4ext::EMainReason::Unload:
    {
        if (!launcher::GetLaunchContext()->enabled)
            return false;

        App::GApplication->Shutdown();
        App::GApplication = nullptr;
        break;
    }
    }

    return true;
}

RED4EXT_C_EXPORT void Query(RED4ext::PluginInfo* aInfo)
{
    aInfo->name = L"CyberpunkMP";
    aInfo->author = L"Tilted Phoques SRL";
    aInfo->version = RED4EXT_SEMVER(0, 1, 0);

    aInfo->runtime = RED4EXT_V0_RUNTIME_INDEPENDENT;
    aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t Supports()
{
    return RED4EXT_API_VERSION_LATEST;
}
