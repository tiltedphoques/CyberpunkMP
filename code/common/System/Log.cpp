#include "Log.h"
#include "Path.h"

Log::Log(Path& aPath)
{
    m_logRoot = aPath.GetRoot() / "logs";
    create_directory(m_logRoot);

    const auto colorSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    colorSink->set_pattern("[\033[97m%Y-%m-%d %H:%M:%S UTC%z\033[0m] [%^%l%$] %v");

    set_default_logger(Create("Server", colorSink));
    Create("Scripting", colorSink);
}

Log::~Log()
{
    spdlog::default_logger()->flush();
}

std::shared_ptr<spdlog::logger> Log::Create(const String& aName, spdlog::sink_ptr aExtraSink, const String& aPattern)
{
    if (auto existingLogger = spdlog::get(aName.c_str()))
        return existingLogger;

    std::filesystem::path path = m_logRoot / (aName + ".log").c_str();

    const auto rotSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path.string(), 1048576 * 5, 3);
    auto logger = std::make_shared<spdlog::logger>(aName.c_str(), spdlog::sinks_init_list{rotSink});
    logger->set_pattern(aPattern.c_str());

    if (aExtraSink)
        logger->sinks().emplace_back(aExtraSink);

#ifdef TP_DEBUG
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::trace);
#else
    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::warn);
#endif

    register_logger(logger);
    return logger;
}
