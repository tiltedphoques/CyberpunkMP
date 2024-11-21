#include "ModuleImage.hpp"

Core::ModuleImage::ModuleImage(HMODULE aHandle)
{
    std::wstring filePath(MAX_PATH, '\0');
    filePath.resize(::GetModuleFileNameW(aHandle, filePath.data(), MAX_PATH));

    m_path = filePath;
}

std::filesystem::path Core::ModuleImage::GetPath() const
{
    return m_path;
}

std::filesystem::path Core::ModuleImage::GetDir() const
{
    return m_path.parent_path();
}

std::string Core::ModuleImage::GetName() const
{
    return m_path.stem().string();
}

bool Core::ModuleImage::IsASI() const
{
    return m_path.extension() == L".asi";
}
