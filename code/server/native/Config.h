#pragma once

#include "Scripting/IConfig.h"

struct FlecsConfig
{
    bool Enabled{true};
    uint16_t Port{27750};
    std::string IpAddress{"127.0.0.1"};

    bool IsEnabled() const { return Enabled; }
    uint16_t GetPort() const { return Port; }
    const char* GetIpAddress() const { return IpAddress.c_str(); }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FlecsConfig, Enabled, Port, IpAddress)
};

struct Config : IConfig
{
    std::string Name{"Default"};
    std::string Description{};
    std::string ApiKey{};
    std::string IconUrl{};
    uint16_t MaxPlayer{4};
    std::string Tags{};
    bool Public{false};
    uint16_t Port{11778};
    uint16_t WebPort{11779};
    uint16_t TickRate{60};
    uint16_t UpdateRate{10};
    std::string Password{};
    FlecsConfig Flecs{};

    const char* GetName() const override { return Name.c_str(); }
    const char* GetDescription() const override { return Description.c_str(); }
    const char* GetApiKey() const override { return ApiKey.c_str(); }
    const char* GetIconUrl() const override { return IconUrl.c_str(); }
    uint16_t GetMaxPlayer() const override { return MaxPlayer; }
    const char* GetTags() const override { return Tags.c_str(); }
    bool GetPublic() const override { return Public; }
    uint16_t GetPort() const override { return Port; }
    uint16_t GetWebPort() const override { return WebPort; }
    uint16_t GetTickRate() const override { return TickRate; }
    uint16_t GetUpdateRate() const override { return UpdateRate; }
    const char* GetPassword() const { return Password.c_str(); }
    const FlecsConfig& GetFlecsConfig() const { return Flecs; }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, Name, Description, IconUrl, MaxPlayer, Tags, TickRate, UpdateRate, Public, Port, Password, ApiKey, Flecs)
};