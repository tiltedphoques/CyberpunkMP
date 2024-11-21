#include "Path.h"

Path::Path()
{
    char path[4096];
#ifdef WIN32
    auto size = GetModuleFileNameA(GetModuleHandleA(nullptr), path, (DWORD)std::size(path));
    std::filesystem::path exePath = path;

    m_root = exePath.parent_path();
#else
    m_root = std::filesystem::current_path();
#endif

    m_mods = m_root / "plugins";
}

const std::filesystem::path Path::GetRoot() const
{
    return m_root;
}

const std::filesystem::path Path::GetModsRoot() const
{
    return m_mods;
}

std::filesystem::path GetAbsolutePath(std::filesystem::path aFilePath, const std::filesystem::path& acRootPath, const bool acAllowNonExisting, const bool acAllowSymlink)
{
    assert(!aFilePath.empty());
    if (aFilePath.empty())
        return {};

    aFilePath.make_preferred();

    if (aFilePath.is_relative())
    {
        if (!acRootPath.empty())
            aFilePath = acRootPath / aFilePath;

        aFilePath = absolute(aFilePath);
    }

    if (is_symlink(aFilePath))
    {
        if (acAllowSymlink)
            return absolute(read_symlink(aFilePath));

        return {};
    }

    if (!exists(aFilePath))
    {
        if (!acAllowNonExisting)
            return {};
    }

    return aFilePath;
}
