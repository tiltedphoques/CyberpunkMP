#include <windows.h>

#include "Entry.h"
#include "App.h"

#include <thread>
#include <mutex>

#include <Core/Platform.h>
#include "FunctionHook.h"

static std::unique_ptr<App> g_pApp;

App& App::GetInstance() noexcept
{
    return *g_pApp;
}

using TWinMain = int(__stdcall)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
TWinMain* OriginalWinMain = nullptr;

static int __stdcall HookedWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Ensure exceptions won't cause our calls to be skipped
    struct ScopedCaller
    {
        ScopedCaller()
        {
            App::GetInstance().BeginMain();

            TP_HOOK_COMMIT
        }
        ~ScopedCaller()
        {
            App::GetInstance().EndMain();
        }
    };

    ScopedCaller appCaller;

    return OriginalWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

static void SetupMainHook()
{
    OriginalWinMain = static_cast<TWinMain*>(App::GetInstance().GetMainAddress());
    if (OriginalWinMain == nullptr)
        return;

    TP_HOOK_IMMEDIATE(&OriginalWinMain, HookedWinMain);
}

static std::once_flag s_mainHookCallFlag;

#if TP_PLATFORM_64

using TGetWinmain = char* (__stdcall*)();
using T_initterm = decltype(&::_initterm);

static TGetWinmain OriginalGetWinmain = nullptr;
static T_initterm Original_initterm = nullptr;

char* __stdcall HookGetWinmain()
{
    std::call_once(s_mainHookCallFlag, SetupMainHook);

    return OriginalGetWinmain();
}

void Hookinitterm(_PVFV* apStart, _PVFV* apEnd) noexcept
{
    std::call_once(s_mainHookCallFlag, SetupMainHook);

    Original_initterm(apStart, apEnd);
}

#else

using TGetStartupInfoA = void(__stdcall*)(LPSTARTUPINFO lpStartupInfo);
TGetStartupInfoA OriginalGetStartupInfoA = nullptr;

void __stdcall HookedGetStartupInfoA(LPSTARTUPINFO lpStartupInfo)
{
    std::call_once(s_mainHookCallFlag, SetupMainHook);

    OriginalGetStartupInfoA(lpStartupInfo);
}

#endif



BOOL details::ReverseMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved, const std::function<std::unique_ptr<App>()>& aAppFactory) noexcept
{
    TP_UNUSED(hModule);
    TP_UNUSED(lpReserved);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        g_pApp = aAppFactory();
#if TP_PLATFORM_64
        OriginalGetWinmain = reinterpret_cast<TGetWinmain>(TP_HOOK_SYSTEM("api-ms-win-crt-runtime-l1-1-0.dll", "_get_narrow_winmain_command_line", HookGetWinmain));
        Original_initterm = reinterpret_cast<T_initterm>(TP_HOOK_SYSTEM("msvcr110.dll", "_initterm", Hookinitterm));
#else
        OriginalGetStartupInfoA = reinterpret_cast<TGetStartupInfoA>(TP_HOOK_SYSTEM("kernel32.dll", "GetStartupInfoA", HookedGetStartupInfoA));
#endif

        App::GetInstance().Attach();

        TP_HOOK_COMMIT

        break;
    }
    case DLL_PROCESS_DETACH:
    {
        App::GetInstance().Detach();

        break;
    }
    default: break;
    }

    return TRUE;
}

