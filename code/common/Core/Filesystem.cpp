#include "Filesystem.h"
#include "Platform.h"

#include <fstream>

#if TP_PLATFORM_WINDOWS
#include <windows.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif


std::filesystem::path GetPath() noexcept
{
#if TP_PLATFORM_WINDOWS
    WCHAR dllPath[MAX_PATH] = { 0 };
    GetModuleFileNameW(reinterpret_cast<HINSTANCE>(&__ImageBase), dllPath, std::size(dllPath));

    std::error_code ec;
    const auto currentPath = std::filesystem::path(dllPath).parent_path();

    return currentPath;
#else
    return std::filesystem::current_path();
#endif
}

String LoadFile(const std::filesystem::path& acPath) noexcept
{
    std::ifstream file(acPath, std::ios::binary);
    file.seekg(0, std::ifstream::end);
    const size_t length = file.tellg();
    file.seekg(0, std::ifstream::beg);

    String content(length, '\0');
    file.read(content.data(), length);

    return content;
}

bool SaveFile(const std::filesystem::path& acPath, const String& acData) noexcept
{
    std::ofstream out(acPath, std::ios::binary);
    if (!out.is_open())
        return false;

    out.write(acData.data(), acData.size());

    return true;
}

