#include "LoggerScriptInstance.h"
#include <GameServer.h>


LoggerScriptInstance::LoggerScriptInstance(const char* aName)
{
    if (strlen(aName) == 0)
    {
        m_logger = spdlog::default_logger();
        return;
    }

    const auto colorSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    colorSink->set_pattern("[\033[97m%Y-%m-%d %H:%M:%S UTC%z\033[0m] [%^%l%$] [Plugin:%n] %v");
    m_logger = GServer->GetLog()->Create(aName, colorSink, "[%Y-%m-%d %H:%M:%S UTC%z] [Plugin:%n] [%^%l%$] %v");
}

void LoggerScriptInstance::Debug(const char* aMessage)
{
    m_logger->debug(aMessage);
}

void LoggerScriptInstance::Info(const char* aMessage)
{
    m_logger->info(aMessage);
}

void LoggerScriptInstance::Warn(const char* aMessage)
{
    m_logger->warn(aMessage);
}

void LoggerScriptInstance::Error(const char* aMessage)
{
    m_logger->error(aMessage);
}


// Refactor this, move it somewhere else
TP_EXPORT ILogger* ILogger::Get(const char* aName)
{
    static std::mutex s_lock;
    // Use unique_ptr so that moves don't invalidate previous pointers
    static Map<String, UniquePtr<LoggerScriptInstance>> m_loggers;

    std::scoped_lock _{s_lock};

    auto it = m_loggers.find(aName);
    if (it != std::end(m_loggers))
        return it.value().get();

    return m_loggers.emplace(aName, MakeUnique<LoggerScriptInstance>(aName)).first.value().get();
}
