#pragma once

struct Path
{
    Path();
    const std::filesystem::path GetRoot() const;
    const std::filesystem::path GetModsRoot() const;

private:
    std::filesystem::path m_root;
    std::filesystem::path m_mods;
};

std::filesystem::path GetAbsolutePath(std::filesystem::path aFilePath, const std::filesystem::path& acRootPath, const bool acAllowNonExisting, const bool acAllowSymlink);
