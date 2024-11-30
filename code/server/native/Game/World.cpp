#include "World.h"

#include "Config.h"
#include "PlayerManager.h"
#include "Level.h"

#include "Components/MovementComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/AttachmentComponent.h"
#include "Components/VehicleComponent.h"

#include "Systems/ChatSystem.h"
#include "Systems/ServerListSystem.h"

World::World(const FlecsConfig& acFlecsConfig)
{
    set_entity_range(1, 5'000'000);

    emplace<Level>(this);
    emplace<PlayerManager>(this);
    emplace<ChatSystem>(this);
    emplace<ServerListSystem>(this);

    if (acFlecsConfig.IsEnabled())
    {
        set<flecs::Rest>({
            .port = acFlecsConfig.GetPort(),
            .ipaddr = const_cast<char*>(acFlecsConfig.GetIpAddress()),
            .impl = nullptr
        });
        spdlog::info("Running Flecs REST API on {}:{}", acFlecsConfig.IpAddress, acFlecsConfig.Port);
    }

    this->import<flecs::units>();
    this->import<flecs::stats>();

    component<std::string>()
        .opaque(flecs::String)
        .serialize(
            [](const flecs::serializer* s, const std::string* data)
            {
                const char* str = data->c_str();
                return s->value(flecs::String, &str); // Forward to serializer
            })
        .assign_string(
            [](std::string* data, const char* value)
            {
                *data = value; // Assign new value to std::string
            });

    MovementComponent::Register(*this);
    AttachmentComponent::Register(*this);
    PlayerComponent::Register(*this);
    VehicleComponent::Register(*this);
}

World::~World()
{
}

void World::Update(float aDelta)
{
    progress(aDelta);
}

gsl::not_null<WorldScriptInstance*> World::GetScriptInstance() noexcept
{
    return &m_scriptInstance;
}

gsl::not_null<const WorldScriptInstance*> World::GetScriptInstance() const noexcept
{
    return &m_scriptInstance;
}
