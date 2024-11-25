#pragma once

struct PlayerComponent
{
    ConnectionId Connection;
    flecs::entity Puppet;
    std::string Username;

    const char* GetUsername() const;

    static void Register(flecs::world& aWorld);
};
