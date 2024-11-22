#include "ErrorHandler.h"

void ErrorHandler::RecordError(absl::string_view filename, int line, int column, absl::string_view message)
{
    std::string fullPath(filename.data(), filename.size());
#if TP_PLATFORM_WINDOWS
    std::ranges::replace(fullPath, '/', '\\');
#endif

    fullPath = (std::filesystem::current_path() / fullPath).string();
    throw std::runtime_error(fmt::format("{}({},{}): error 0: {}\n", fullPath, line, column, message));
}
