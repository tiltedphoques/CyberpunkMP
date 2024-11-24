#include "Scripting.h"

#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>

#include "System/Path.h"

#ifdef WIN32
#define STR(s) L##s
#else
#include <dlfcn.h>
#define STR(s) s
#define MAX_PATH 2048
#endif

namespace
{
const auto dotnetType = STR("CyberpunkSdk.Server, CyberpunkSdk");

#ifdef WIN32
void* LoadSharedLibrary(const char_t* path)
{
    HMODULE h = ::LoadLibraryW(path);
    assert(h != nullptr);
    return (void*)h;
}
void* GetExport(void* h, const char* name)
{
    void* f = ::GetProcAddress(static_cast<HMODULE>(h), name);
    assert(f != nullptr);
    return f;
}
#else
void* LoadSharedLibrary(const char_t* path)
{
    void* h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
    assert(h != nullptr);
    return h;
}
void* GetExport(void* h, const char* name)
{
    void* f = dlsym(h, name);
    assert(f != nullptr);
    return f;
}
#endif

bool LoadHostFxr(hostfxr_initialize_for_runtime_config_fn& apInitialize, hostfxr_get_runtime_delegate_fn& apGetDelegate, hostfxr_close_fn& apClose)
{
    // Pre-allocate a large buffer for the path to hostfxr
    char_t buffer[MAX_PATH];
    size_t buffer_size = sizeof(buffer) / sizeof(char_t);
    const int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
    if (rc != 0)
        return false;

    // Load hostfxr and get desired exports
    void* lib = LoadSharedLibrary(buffer);
    apInitialize = (hostfxr_initialize_for_runtime_config_fn)GetExport(lib, "hostfxr_initialize_for_runtime_config");
    apGetDelegate = (hostfxr_get_runtime_delegate_fn)GetExport(lib, "hostfxr_get_runtime_delegate");
    apClose = (hostfxr_close_fn)GetExport(lib, "hostfxr_close");

    return (apInitialize && apGetDelegate && apClose);
}

std::tuple<load_assembly_and_get_function_pointer_fn, get_function_pointer_fn>
GetDotnetLoadAssembly(const char_t* config_path, hostfxr_initialize_for_runtime_config_fn apInitialize, hostfxr_get_runtime_delegate_fn apGetDelegate, hostfxr_close_fn apClose)
{
    if (!exists(std::filesystem::path(config_path)))
    {
        std::ofstream fs(config_path);
        fs << R"V0G0N({
  "runtimeOptions": {
    "tfm": "net8.0",
    "framework": {
      "name": "Microsoft.NETCore.App",
      "version": "8.0.1"
    },
    "configProperties": {
      "System.Reflection.Metadata.MetadataUpdater.IsSupported": false
    }
  }
})V0G0N";
    }

    // Load .NET Core
    void* pLoadAssemblyAndGetFunctionPointer = nullptr;
    void* pGetFunctionPointer = nullptr;
    hostfxr_handle pContext = nullptr;
    int rc = apInitialize(config_path, nullptr, &pContext);
    if (rc != 0 || pContext == nullptr)
    {
        spdlog::get("Scripting")->error("Failed to initialize .net runtime, with error {:X}", rc);
        apClose(pContext);
        return {nullptr, nullptr};
    }

    // Get the load assembly function pointer
    rc = apGetDelegate(pContext, hdt_load_assembly_and_get_function_pointer, &pLoadAssemblyAndGetFunctionPointer);
    if (rc != 0 || pLoadAssemblyAndGetFunctionPointer == nullptr)
        spdlog::get("Scripting")->error("Get delegate failed, with error {:X}", rc);

    rc = apGetDelegate(pContext, hdt_get_function_pointer, &pGetFunctionPointer);
    if (rc != 0 || pGetFunctionPointer == nullptr)
        spdlog::get("Scripting")->error("Get delegate failed, with error {:X}", rc);

    apClose(pContext);

    return std::make_tuple(
        reinterpret_cast<load_assembly_and_get_function_pointer_fn>(pLoadAssemblyAndGetFunctionPointer), reinterpret_cast<get_function_pointer_fn>(pGetFunctionPointer));
}
} // namespace

Scripting::Scripting(Path& aPath)
    : m_path(aPath)
{
    hostfxr_initialize_for_runtime_config_fn pInitialize = nullptr;
    hostfxr_get_runtime_delegate_fn pGetDelegate = nullptr;
    hostfxr_close_fn pClose = nullptr;

    if (!LoadHostFxr(pInitialize, pGetDelegate, pClose))
    {
        spdlog::get("Scripting")->error("Failed to initialize Dotnet");
        return;
    }

    const auto config_path = aPath.GetRoot() / "dotnet.runtimeconfig.json";
    std::tie(m_pLoadAssembly, m_pGetFnPtr) = GetDotnetLoadAssembly(config_path.c_str(), pInitialize, pGetDelegate, pClose);

    if (!m_pLoadAssembly || !m_pGetFnPtr)
    {
        spdlog::get("Scripting")->error("Failed to initialize Dotnet load assembly, is 'dotnet.runtimeconfig.json' missing?");
        return;
    }

    spdlog::get("Scripting")->debug("Dotnet loaded successfully.");
}

void Scripting::LoadPlugins()
{
    namespace fs = std::filesystem;

    spdlog::get("Scripting")->debug("Started loading plugins.");

    const auto apiPath = m_path.GetRoot() / "plugins";
    create_directories(apiPath);

    m_sdkPath = apiPath / ("CyberpunkSdk.dll");
    if (!exists(m_sdkPath))
    {
        spdlog::get("Scripting")->warn("Could not find {}", m_sdkPath.string());
        spdlog::get("Scripting")->warn("Scripting disabled.");
        return;
    }
    spdlog::get("Scripting")->debug("Found {}", m_sdkPath.string());

    m_pInitialize = GetFunction(L"Initialize");
}

void Scripting::Initialize()
{
    int result = -1;
    if (m_pInitialize != nullptr)
    {
        result = m_pInitialize(nullptr, 0);
    }
    if (result != 0)
    {
        spdlog::get("Scripting")->error("SDK failed to Initialize with error {}", result);
        spdlog::get("Scripting")->warn("Scripting disabled.");
        return;
    }

    spdlog::get("Scripting")->debug("Done loading plugins.");
}

component_entry_point_fn Scripting::GetFunction(const wchar_t* apName) const
{
#if TP_PLATFORM_WINDOWS
    const auto* pName = apName;
#else
    const size_t cBufferSize = wcstombs(nullptr, apName, 0) + 1;
    char pName[cBufferSize];
    std::wcstombs(pName, apName, cBufferSize);
#endif

    void* out;
    uint32_t rc = m_pLoadAssembly(m_sdkPath.c_str(), dotnetType, pName, nullptr, nullptr, &out);
    if (rc != 0)
    {
        return nullptr;
    }

    return reinterpret_cast<component_entry_point_fn>(out);
}
