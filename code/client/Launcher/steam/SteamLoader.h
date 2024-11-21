#pragma once

#include <Core/Filesystem.h>

namespace steam
{
namespace fs = std::filesystem;

void Load(const fs::path& aGameDir);
} // namespace steam
