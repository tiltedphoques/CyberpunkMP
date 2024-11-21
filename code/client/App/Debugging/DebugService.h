#pragma once

#include "Core/Foundation/Feature.hpp"
#include "Core/Logging/LoggingAgent.hpp"

namespace App
{
struct DebugService : Core::Feature, Core::LoggingAgent
{
    DebugService();
    ~DebugService() override;

    void OnBootstrap() override;

    void Draw();
};
} // namespace App
