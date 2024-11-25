#pragma once

struct World;
struct ServerListSystem
{
    ServerListSystem(gsl::not_null<World*> apWorld);

private:
    void Tick() noexcept;
    void Announce() noexcept;

    static void PostAnnouncement(
        const std::string& acName, const std::string& acDesc, const std::string& acIconUrl, uint16_t aPort, uint16_t aTick, uint16_t aPlayerCount, uint16_t aPlayerMaxCount,
        const std::string& acTagList, bool aPublic, bool aPassword, int32 aFlags) noexcept;

    gsl::not_null<World*> m_pWorld;
    flecs::system m_updateSystem;
    flecs::observer m_serverListObserver;
    mutable std::chrono::steady_clock::time_point m_nextAnnounce;
};
