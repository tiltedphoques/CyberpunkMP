#include "PlayerComponent.h"

void PlayerComponent::Register(flecs::world& aWorld)
{
    aWorld.component<PlayerComponent>()
        .member("Connection", &PlayerComponent::Connection)
        .member("Puppet", &PlayerComponent::Puppet)
        .member("Username", &PlayerComponent::Username);
}

const char* PlayerComponent::GetUsername() const
{
    return Username.c_str();
}
