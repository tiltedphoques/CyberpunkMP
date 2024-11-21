#include "ErrorHandler.h"

void ErrorHandler::RecordError(absl::string_view filename, int line, int column, absl::string_view message)
{
#if TP_PLATFORM_WINDOWS
    std::string fullPath(filename.data(), filename.size());
    std::ranges::replace(fullPath, '/', '\\');

    fullPath = (std::filesystem::current_path() / fullPath).string();

#endif
    throw std::runtime_error(fmt::format("{}({},{}): error 0: {}\n", fullPath, line, column, message));
}
