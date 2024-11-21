#pragma once
#include <coreclr_delegates.h>

struct Path;
struct Scripting
{
    Scripting(Path& aPath);
    ~Scripting() = default;

    void LoadPlugins();
    void Initialize();

    component_entry_point_fn GetFunction(const wchar_t* apName) const;

private:

    Path& m_path;
    load_assembly_and_get_function_pointer_fn m_pLoadAssembly{nullptr};
    get_function_pointer_fn m_pGetFnPtr{nullptr};
    std::filesystem::path m_sdkPath;

    component_entry_point_fn m_pInitialize{nullptr};
};