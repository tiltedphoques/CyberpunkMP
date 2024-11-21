
#include <strsafe.h>

#include <MinHook.h>
#include <winternl.h>

#include "DllBlocklist.h"
#include "Launcher/TargetConfig.h"
#include "Launcher/utils/NtInternal.h"
#include "Launcher/utils/Error.h"
#include "Launcher/utils/IAT.h"
#include "Launcher/launcher.h"

#include <Shlobj.h>

#include "RED4ext/Api/EMainReason.hpp"
#include "RED4ext/Api/Sdk.hpp"

// defined in MemoryLayout.cpp
extern bool IsThisExeAddress(const uint8_t* apAddress);
extern bool IsGameMemoryAddress(const uint8_t* apAddress);

extern "C" __declspec(dllimport) NTSTATUS WINAPI LdrGetDllFullName(HMODULE, PUNICODE_STRING);

namespace
{
std::wstring s_OverridePath;

DWORD(WINAPI* RealGetModuleFileNameW)(HMODULE, LPWSTR, DWORD) = nullptr;
DWORD(WINAPI* RealGetModuleFileNameA)(HMODULE, LPSTR, DWORD) = nullptr;

HMODULE(WINAPI* RealGetModuleHandleW)(LPCWSTR) = nullptr;
HMODULE(WINAPI* RealGetModuleHandleA)(LPCSTR) = nullptr;
BOOL(WINAPI* RealGetModuleHandleExA)(DWORD, LPCSTR, HMODULE*) = nullptr;
BOOL(WINAPI* RealGetModuleHandleExW)(DWORD, LPCWSTR, HMODULE*) = nullptr;

NTSTATUS(WINAPI* RealLdrLoadDll)(const wchar_t*, uint32_t*, UNICODE_STRING*, HANDLE*) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllHandle)(PWSTR, PULONG, PUNICODE_STRING, PVOID*) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllFullName)(HMODULE, PUNICODE_STRING) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllHandleEx)
(ULONG Flags, PWSTR DllPath, PULONG DllCharacteristics, UNICODE_STRING* DllName, PVOID* DllHandle) = nullptr;

inline bool IsMyModule(HMODULE aHmod)
{
    return aHmod == nullptr || aHmod == NtInternal::ThePeb()->pImageBase;
}

HMODULE HModFromAddress(void* apAddress)
{
    HMODULE pModule = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(apAddress), &pModule);

    return pModule;
}

DWORD MYGetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
    if (!nSize)
        return 0;

    if (nSize > 0x7FFF)
        nSize = 0x7FFF;

    UNICODE_STRING str{};
    str.Buffer = lpFilename;
    str.MaximumLength = static_cast<USHORT>(sizeof(wchar_t) * nSize - sizeof(wchar_t));

    ::LdrGetDllFullName(hModule, &str);

    DWORD nChars = str.Length / sizeof(wchar_t);
    str.Buffer[nChars] = 0;
    return nChars;
}

bool IsLocalModulePath(HMODULE aHmod)
{
    std::wstring buf;

    size_t i = 1;
    DWORD res = 0;
    do
    {
        buf.resize(i * MAX_PATH);
        res = MYGetModuleFileNameW(aHmod, buf.data(), static_cast<DWORD>(buf.length()));
        i++;
    } while (res == ERROR_INSUFFICIENT_BUFFER);

    std::wstring buf2;
    buf2.resize(buf.length() * 2);
    memset(buf2.data(), 0, buf2.length() * 2);
    buf2 = buf;

    ::OutputDebugStringW(buf2.c_str());
    // does the file exist in the ST dir?
    return buf.find(s_OverridePath) != std::wstring::npos;
}

// some mods do GetModuleHandle("SkyrimSE.exe") for some reason instead of GetModuleHandle(nullptr)
NTSTATUS WINAPI TP_LdrGetDllHandle(PWSTR DllPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PVOID* DllHandle)
{
    if (std::wcsncmp(TARGET_NAME L".exe", DllName->Buffer, DllName->Length) == 0)
    {
        *DllHandle = NtInternal::ThePeb()->pImageBase;
        return 0; // success
    }

    return RealLdrGetDllHandle(DllPath, DllCharacteristics, DllName, DllHandle);
}

NTSTATUS WINAPI TP_LdrGetDllHandleEx(ULONG Flags, PWSTR DllPath, PULONG DllCharacteristics, UNICODE_STRING* DllName, PVOID* DllHandle)
{
    if (DllName && std::wcsncmp(TARGET_NAME L".exe", DllName->Buffer, DllName->Length) == 0)
    {
        *DllHandle = NtInternal::ThePeb()->pImageBase;
        return 0; // success
    }

    return RealLdrGetDllHandleEx(Flags, DllPath, DllCharacteristics, DllName, DllHandle);
}

NTSTATUS WINAPI TP_LdrGetDllFullName(HMODULE Module, PUNICODE_STRING DllName)
{
    void* rsp = _ReturnAddress();

    // we need to detect if GetModuleFileName(null) was called from our own code or the game code/other modules
    // if (!GetTls()->inLoadDll)
    if (launcher::GetLaunchContext())
    {
        if (Module == nullptr || Module == NtInternal::ThePeb()->pImageBase)
        {
            auto& aExePath = launcher::GetLaunchContext()->exePath;
            RtlInitUnicodeString(DllName, aExePath.c_str());
            return 0; // sucess
        }
    }

    return RealLdrGetDllFullName(Module, DllName);
}

bool NeedsToFool(void* pRbp, bool* wantsTruth = nullptr)
{
    // game code/stub segment within this exe needs to be fooled
    if (IsThisExeAddress(static_cast<uint8_t*>(pRbp)))
    {
        return IsGameMemoryAddress(static_cast<uint8_t*>(pRbp));
    }

    HMODULE hMod = HModFromAddress(pRbp);
    if (hMod == NtInternal::ThePeb()->pImageBase || hMod == nullptr /*This is a hook, virtual allocd, not owned by anybody, so we assign ownership to the ST directory*/)
    {
        if (wantsTruth)
            *wantsTruth = true;
        return false;
    }

    return !IsLocalModulePath(hMod);
}

DWORD WINAPI TP_GetModuleFileNameW(HMODULE aModule, LPWSTR alpFilename, DWORD aSize)
{
    void* rbp = _ReturnAddress();

    // PrintOwnerNa me(rbp);

    bool force = false;
    if (IsMyModule(aModule) && NeedsToFool(rbp, &force) && launcher::GetLaunchContext())
    {
        auto& aExePath = launcher::GetLaunchContext()->exePath;
        StringCchCopyW(alpFilename, aSize, aExePath.c_str());

        return static_cast<DWORD>(std::wcslen(alpFilename));
    }

    if (force)
        return MYGetModuleFileNameW(aModule, alpFilename, aSize);

    return RealGetModuleFileNameW(aModule, alpFilename, aSize);
}

struct ScopedOSHeapItem
{
    ScopedOSHeapItem(size_t aSize) { m_pBlock = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, aSize); }

    ~ScopedOSHeapItem()
    {
        if (m_pBlock)
            HeapFree(GetProcessHeap(), 0, m_pBlock);
    }

    void* m_pBlock;
};

// NOTE(Vince): Introduce one layer of indirection by calling GetModuleFileNameW function directly, to trigger usvfs
// indirection.
DWORD WINAPI TP_GetModuleFileNameA(HMODULE aModule, char* alpFileName, DWORD aBufferSize)
{
    void* rbp = _ReturnAddress();
    if (IsMyModule(aModule) && NeedsToFool(rbp) && launcher::GetLaunchContext())
    {
        auto aExePath = launcher::GetLaunchContext()->exePath.string();
        StringCchCopyA(alpFileName, aBufferSize, aExePath.c_str());

        return static_cast<DWORD>(std::strlen(alpFileName));
    }

    ScopedOSHeapItem wideBuffer((aBufferSize * sizeof(wchar_t)) + 1);

    wchar_t* pBuffer = static_cast<wchar_t*>(wideBuffer.m_pBlock);
    DWORD result = RealGetModuleFileNameW(aModule, pBuffer, aBufferSize * sizeof(wchar_t));
    if (result == 0)
    {
        return result;
    }

    UNICODE_STRING source{};
    RtlInitUnicodeString(&source, pBuffer);

    // convert using the proper OS function
    ANSI_STRING dest{.Length = static_cast<USHORT>(result), .MaximumLength = static_cast<USHORT>(aBufferSize), .Buffer = alpFileName};
    if (RtlUnicodeStringToAnsiString(&dest, &source, FALSE) != 0)
        return 0;

    return result;
}

// we use this function to enforce the DLL load policy
NTSTATUS WINAPI TP_LdrLoadDll(const wchar_t* apPath, uint32_t* apFlags, UNICODE_STRING* apFileName, HANDLE* apHandle)
{
    std::wstring_view fileName(apFileName->Buffer, apFileName->Length / sizeof(wchar_t));
    size_t pos = fileName.find_last_of(L'\\');
    if (pos != std::wstring_view::npos && (pos + 1) != fileName.length())
    {
        if (stubs::IsDllBlocked(&fileName[pos + 1]))
        {
            // invalid image hash
            // this signals windows to *NOT TRY* loading it again at a later time.
            return 0xC0000428;
        }
    }

    return RealLdrLoadDll(apPath, apFlags, apFileName, apHandle);
}

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
HMODULE TP_GetModuleHandleA(const char* lpModuleName)
{
    if (!lpModuleName)
    {
        return (HMODULE)&__ImageBase;
    }

    return RealGetModuleHandleA(lpModuleName);
}

HMODULE TP_GetModuleHandleW(const wchar_t* lpModuleName)
{
    if (!lpModuleName)
    {
        return (HMODULE)&__ImageBase;
    }

    return RealGetModuleHandleW(lpModuleName);
}
BOOL TP_GetModuleHandleExA(DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule)
{
    if (!lpModuleName)
    {
        *phModule = (HMODULE)&__ImageBase;
        return TRUE;
    }

    return RealGetModuleHandleExA(dwFlags, lpModuleName, phModule);
}

BOOL TP_GetModuleHandleExW(DWORD dwFlags, const wchar_t* lpModuleName, HMODULE* phModule)
{
    if (!lpModuleName)
    {
        *phModule = (HMODULE)&__ImageBase;
        return TRUE;
    }

    return RealGetModuleHandleExW(dwFlags, lpModuleName, phModule);
}

DWORD(WINAPI* RealGetModuleFileNameExW)(HANDLE, HMODULE, LPWSTR, DWORD) = nullptr;
DWORD WINAPI GetModuleFileNameExW_Hook(HANDLE hProcess, HMODULE hModule, LPWSTR alpFilename, DWORD aSize)
{
    void* rbp = _ReturnAddress();
    // PrintOwnerNa me(rbp);

    bool force = false;
    if (IsMyModule(hModule) && NeedsToFool(rbp, &force) && launcher::GetLaunchContext())
    {
        auto& aExePath = launcher::GetLaunchContext()->exePath;
        StringCchCopyW(alpFilename, aSize, aExePath.c_str());

        return static_cast<DWORD>(std::wcslen(alpFilename));
    }

    return RealGetModuleFileNameExW(hProcess, hModule, alpFilename, aSize);
}

DWORD(WINAPI* RealGetModuleFileNameExA)(HANDLE, HMODULE, char*, DWORD) = nullptr;
DWORD WINAPI GetModuleFileNameExA_Hook(HANDLE hProcess, HMODULE aModule, char* alpFileName, DWORD aBufferSize)
{
    void* rbp = _ReturnAddress();
    if (IsMyModule(aModule) && NeedsToFool(rbp) && launcher::GetLaunchContext())
    {
        auto aExePath = launcher::GetLaunchContext()->exePath.string();
        StringCchCopyA(alpFileName, aBufferSize, aExePath.c_str());

        return static_cast<DWORD>(std::strlen(alpFileName));
    }

    ScopedOSHeapItem wideBuffer((aBufferSize * sizeof(wchar_t)) + 1);

    wchar_t* pBuffer = static_cast<wchar_t*>(wideBuffer.m_pBlock);
    DWORD result = RealGetModuleFileNameW(aModule, pBuffer, aBufferSize * sizeof(wchar_t));
    if (result == 0)
    {
        return result;
    }

    UNICODE_STRING source{};
    RtlInitUnicodeString(&source, pBuffer);

    // convert using the proper OS function
    ANSI_STRING dest{.Length = static_cast<USHORT>(result), .MaximumLength = static_cast<USHORT>(aBufferSize), .Buffer = alpFileName};
    if (RtlUnicodeStringToAnsiString(&dest, &source, FALSE) != 0)
        return 0;

    return result;
}
HRESULT (*Real_SHGetKnownFolderPath)(REFKNOWNFOLDERID, DWORD, HANDLE, PSTR*);
HRESULT TP_SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PSTR* ppszPath)
{
    auto rval = Real_SHGetKnownFolderPath(rfid, dwFlags, hToken, ppszPath);
    return rval;
}

} // namespace

#define VALIDATE(x) \
    if (x != MH_OK) \
        Die(L"CoreStubsInit(): Fatal Minhook error.", true);

//bool Main(RED4ext::PluginHandle aHandle, RED4ext::EMainReason aReason, const RED4ext::Sdk* aSdk);
//void Query(RED4ext::PluginInfo* aInfo);
//uint32_t Supports();

void* (*Real_GetProcAddress)(HMODULE hModule, LPCSTR lpProcName) = nullptr;
void* TP_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    /*if (hModule == static_cast<void*>(&__ImageBase))
    {
        if (strcmp(lpProcName, "Supports") == 0)
            return &Supports;

        if (strcmp(lpProcName, "Query") == 0)
            return &Query;

        if (strcmp(lpProcName, "Main") == 0)
            return &Main;
    }*/

    return Real_GetProcAddress(hModule, lpProcName);
}

// pre eat hook?? loadmodule hook??
// the idea would be to link against an external dll, which in its init routine then hooks, so we execute before mo2?
void CoreStubsInit()
{
    set_base();

    // Prevent ASI Loader and RED4ext's proxy dll from doing anything
    LoadLibraryA("winmm.dll");
    LoadLibraryA("version.dll");

    s_OverridePath = GetPath().wstring();
    std::replace(s_OverridePath.begin(), s_OverridePath.end(), L'/', L'\\');

    VALIDATE(MH_Initialize());
    VALIDATE(MH_CreateHookApi(L"Kernel32.dll", "GetProcAddress", &TP_GetProcAddress, (void**)&Real_GetProcAddress));
    VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetProcAddress", &TP_GetProcAddress, (void**)&Real_GetProcAddress));

    VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleFileNameA", &TP_GetModuleFileNameA, (void**)&RealGetModuleFileNameA));
    VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleFileNameExA", &GetModuleFileNameExA_Hook, (void**)&RealGetModuleFileNameExA));
    VALIDATE(MH_CreateHookApi(L"Kernel32.dll", "K32GetModuleFileNameExA", &GetModuleFileNameExA_Hook, nullptr));
    VALIDATE(MH_CreateHookApi(L"Kernel32.dll", "K32GetModuleFileNameExW", &GetModuleFileNameExW_Hook, nullptr));

    //VALIDATE(MH_CreateHookApi(L"Kernel32.dll", "VirtualAlloc", &TP_VirtualAlloc, (void**)&Real_VirtualAlloc));
    //VALIDATE(MH_CreateHookApi(L"Kernel32.dll", "HeapAlloc", &TP_HeapAlloc, (void**)&Real_HeapAlloc));

    VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleFileNameW", &TP_GetModuleFileNameW, (void**)&RealGetModuleFileNameW));
    VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleFileNameExW", &GetModuleFileNameExW_Hook, (void**)&RealGetModuleFileNameExW));

    //  K32GetModuleFileNameExA

    VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleHandleA", &TP_GetModuleHandleA, (void**)&RealGetModuleHandleA));
    VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleHandleExA", &TP_GetModuleHandleExA, (void**)&RealGetModuleHandleExA));

    VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleHandleW", &TP_GetModuleHandleW, (void**)&RealGetModuleHandleW));
    VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleHandleExW", &TP_GetModuleHandleExW, (void**)&RealGetModuleHandleExW));

    VALIDATE(MH_CreateHookApi(L"Shell32.dll", "SHGetKnownFolderPath", &TP_SHGetKnownFolderPath, (void**)&Real_SHGetKnownFolderPath));

    //VALIDATE(MH_CreateHookApi(L"ntdll.dll", "LdrGetDllHandle", &TP_LdrGetDllHandle, (void**)&RealLdrGetDllHandle));
    //VALIDATE(MH_CreateHookApi(L"ntdll.dll", "LdrGetDllHandleEx", &TP_LdrGetDllHandleEx, (void**)&RealLdrGetDllHandleEx));

    // TODO(Vince): we need some check if usvfs already fucked with this?
    //MH_CreateHookApi(L"ntdll.dll", "LdrGetDllFullName", &TP_LdrGetDllFullName, (void**)&RealLdrGetDllFullName);
    VALIDATE(MH_CreateHookApi(L"ntdll.dll", "LdrLoadDll", &TP_LdrLoadDll, (void**)&RealLdrLoadDll));

    VALIDATE(MH_EnableHook(nullptr));
}

#undef VALIDATE

void LaterInstall()
{
    /*iat("Kernel32.dll", "K32GetModuleFileNameExA", &GetModuleFileNameExA_Hook, RealGetModuleFileNameExA);
    iat("Kernel32.dll", "K32GetModuleFileNameExW", &GetModuleFileNameExW_Hook, RealGetModuleFileNameExW);

    iat("Kernel32.dll", "GetModuleFileNameA", &TP_GetModuleFileNameA, RealGetModuleFileNameA);
    iat("Kernel32.dll", "GetModuleFileNameExA", &GetModuleFileNameExA_Hook, RealGetModuleFileNameExA);

    iat("Kernel32.dll", "GetModuleFileNameW", &TP_GetModuleFileNameW, RealGetModuleFileNameW);
    iat("Kernel32.dll", "GetModuleFileNameExW", &GetModuleFileNameExW_Hook, RealGetModuleFileNameExW);

    iat("Kernel32.dll", "GetModuleHandleA", &TP_GetModuleHandleA, RealGetModuleHandleA);
    iat("Kernel32.dll", "GetModuleHandleExA", &TP_GetModuleHandleExA, RealGetModuleHandleExA);
    iat("Kernel32.dll", "GetModuleHandleW", &TP_GetModuleHandleW, RealGetModuleHandleW);
    iat("Kernel32.dll", "GetModuleHandleExW", &TP_GetModuleHandleExW, RealGetModuleHandleExW);
    // same for SHGetKnownFolderPath:
    iat("Shell32.dll", "SHGetKnownFolderPath", &TP_SHGetKnownFolderPath, Real_SHGetKnownFolderPath);
    iat("Shell32.dll", "SHGetKnownFolderPathW", &TP_SHGetKnownFolderPathW, Real_SHGetKnownFolderPathW);*/
    // iat("ntdll.dll", "LdrLoadDll", &TP_LdrLoadDll, RealLdrLoadDll);
}
