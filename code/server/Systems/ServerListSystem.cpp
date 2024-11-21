#include "ServerListSystem.h"

#include "GameServer.h"
#include "Components/PlayerComponent.h"

static constexpr char kMasterServerEndpoint[] =
    //"http://127.0.0.1:8000";
    "https://cyberpunk.skyrim-together.com";

ServerListSystem::ServerListSystem(gsl::not_null<World*> apWorld)
    : m_pWorld(apWorld)
    , m_nextAnnounce{}
{
    m_updateSystem = apWorld->system("Server list Update").kind(flecs::OnUpdate).iter([this](flecs::iter& iter) { Tick(); });

    m_serverListObserver = apWorld->observer<PlayerComponent>("Server list player Observer")
                               .event(flecs::OnSet)
                               .event(flecs::OnRemove)
                               .each([this](flecs::iter& it, size_t i, PlayerComponent& component) { m_nextAnnounce = {}; });

    m_updateSystem.child_of(apWorld->entity("systems"));
    m_serverListObserver.child_of(apWorld->entity("observers"));
}

void ServerListSystem::Tick() noexcept
{
    if (m_nextAnnounce < std::chrono::steady_clock::now())
    {
        Announce();

        m_nextAnnounce = std::chrono::steady_clock::now() + std::chrono::minutes(1);
    }
}

void ServerListSystem::Announce() noexcept
{
    std::thread(
        [&]()
        {
            const auto& config = GServer->GetConfig();
            auto pc = static_cast<uint16_t>(m_pWorld->get<PlayerManager>()->Count());
            PostAnnouncement(config->Name, config->Description, config->IconUrl, GServer->GetPort(), GServer->GetTickRate(), pc, 10000, config->Tags, true, false, 0);
        })
        .detach();
}

void ServerListSystem::PostAnnouncement(
    const std::string& acName, const std::string& acDesc, const std::string& acIconUrl, uint16_t aPort, uint16_t aTick, uint16_t aPlayerCount, uint16_t aPlayerMaxCount,
    const std::string& acTagList, bool aPublic, bool aPassword, int32 aFlags) noexcept
{
    const std::string kVersion{"v0.1"};
    const httplib::Params params{
        {"name", acName},
        {"desc", acDesc},
        {"icon_url", acIconUrl},
        {"version", kVersion},
        {"port", std::to_string(aPort)},
        {"tick", std::to_string(aTick)},
        {"player_count", std::to_string(aPlayerCount)},
        {"max_player_count", std::to_string(aPlayerMaxCount)},
        {"tags", acTagList},
        {"public", aPublic ? "true" : "false"},
        {"pass", aPassword ? "true" : "false"},
        {"flags", std::to_string(aFlags)},
    };

    httplib::Client client(kMasterServerEndpoint);
    client.enable_server_certificate_verification(false);
    client.set_read_timeout(std::chrono::milliseconds(30000));
    const auto response = client.Post("/announce", params);

    // If we send a 403 it means we banned this server
    if (response)
    {
        if (response->status == 403)
            GServer->Kill();
        else if (response->status != 200)
            spdlog::error("Server list error! {}", response->body);
    }
    else
    {
        spdlog::error("Server could not reach the server list! {}", to_string(response.error()));
    }
}
