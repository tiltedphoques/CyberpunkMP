
#include "DllBlocklist.h"

namespace stubs
{
// clang-format off

    // A list of modules blocked for a variety of reasons, but mainly
    const wchar_t* const kDllBlocklist[] = {
        //L"crashhandler64.dll",       // Stream crash handler, breaks heap
        L"ReShade64_SpecialK64.dll", // same reason
        L"NvCamera64.dll",           // broken af nvidia stuff, blacklisted for now, needs fix later
        L"CChromaEditorLibrary64.dll",
        L"GFSDK_Aftermath_Lib.x64.dll",
        L"AnselSDK64.dll",

       // L"atiuxp64.dll",
       // L"aticfx64.dll"
    };

// clang-format on

bool IsDllBlocked(std::wstring_view dllName)
{
    for (const wchar_t* dllEntry : kDllBlocklist)
    {
        if (std::wcscmp(dllName.data(), dllEntry) == 0)
        {
            return true;
        }
    }

    return false;
}
} // namespace stubs
