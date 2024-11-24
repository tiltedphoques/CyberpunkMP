#include "ILogger.h"

struct LoggerScriptInstance : ILogger
{
    LoggerScriptInstance(const char* aName);

    void Debug(const char* aMessage) override;
    void Info(const char* aMessage) override;
    void Warn(const char* aMessage) override;
    void Error(const char* aMessage) override;

private:
    std::shared_ptr<spdlog::logger> m_logger;
};
