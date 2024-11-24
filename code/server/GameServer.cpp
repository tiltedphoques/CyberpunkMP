#include "GameServer.h"

#include <Components/PlayerComponent.h>

#include "Game/Level.h"
#include "Scripting/IRpc.h"
#include "Scripting/RpcScriptInstance.h"

#include "PlayerManager.h"


using nlohmann::json;

GameServer* GServer = nullptr;

GameServer::GameServer()
    : Server(client::kIdentifier, server::kIdentifier)
    , m_log(m_path)
    , m_scripting(m_path)
    , m_lastUpdate(std::chrono::steady_clock::now())
{
    GServer = this;

    try
    {
        std::ifstream f("config/server.json");
        if (!f.is_open())
        {
            std::error_code ec;
            std::filesystem::create_directory("config/", ec);

            std::ofstream of("config/server.json");
            json data = m_config;
            of << std::setw(4) << data;
            spdlog::info("Creating config.json as it could not be found!");
        }
        else
        {
            json data = json::parse(f);
            m_config = data.get<Config>();
        }
    }
    catch (std::exception& e)
    {
        spdlog::error("Error parsing config.json: {}", e.what());
        m_run = false;
    }

    uint16_t port = m_config.Port;
    while (!Host(port, m_config.TickRate))
    {
        spdlog::warn("Port {} is already in use, trying {}", port, port + 1);
        port++;
    }

    m_scripting.LoadPlugins();

    m_pWorld = MakeUnique<World>();
    m_pWorld->GetScriptInstance()->Initialize();

    RegisterHandler<&GameServer::HandleAuthentication>(this);

    m_scripting.Initialize();

    spdlog::info("Server started on port {}", GetPort());
}

GameServer::~GameServer()
{
    GServer = nullptr;
}

void GameServer::Kill()
{
    m_run = false;
    Close();
}

void GameServer::Run()
{
    while (m_run && IsListening())
        Update();
}

void GameServer::OnUpdate()
{
    const auto now = std::chrono::steady_clock::now();
    const auto delta = now - m_lastUpdate;
    m_lastUpdate = now;

    m_tasks.Drain();
    m_dispatcher.update();

    m_pWorld->Update(std::chrono::duration_cast<std::chrono::duration<float>>(delta).count());
}

void GameServer::OnConsume(const void* apData, uint32_t aSize, ConnectionId aConnectionId)
{
    // ReSharper disable once CppCStyleCast
    ViewBuffer buffer((uint8_t*)apData, aSize);  // NOLINT(clang-diagnostic-cast-qual)
    Buffer::Reader reader(&buffer);

    if (const auto result = client::Deserializer::Process(reader, aConnectionId, m_dispatcher); !result)
    {
        spdlog::error("Failed to deserialize packet from {}", aConnectionId);
    }
}

void GameServer::OnConnection(ConnectionId aHandle)
{
    spdlog::debug("Connection received {:x}", aHandle);
}

void GameServer::OnDisconnection(ConnectionId aConnectionId, EDisconnectReason aReason)
{
    spdlog::debug("Connection ended {:x}", aConnectionId);

    auto* pPlayerManager = GetWorld()->get_mut<PlayerManager>();

    if (const auto player = pPlayerManager->GetByConnectionId(aConnectionId))
    {
        pPlayerManager->Remove(player);

        if (auto* pPlayerComponent = player.get<PlayerComponent>())
        {
            GetWorld()->get_mut<Level>()->Remove(pPlayerComponent->Puppet);
        }

        player.destruct();

        spdlog::info("{}/{} player(s) online", pPlayerManager->Count(), m_config.MaxPlayer);
    }
}

void GameServer::HandleAuthentication(const PacketEvent<client::AuthenticationRequest>& aRequest)
{
    server::AuthenticationResponse response;

    if (aRequest.get_client_protocol() != client::kIdentifier)
    {
        response.set_success(false);
        response.set_error("Invalid protocol version!");

        spdlog::warn("Connection attempt with client identifier {:x}, expected {:x}", aRequest.get_client_protocol(), client::kIdentifier);
        Send(aRequest.ConnectionId, response);
        Kick(aRequest.ConnectionId);
        return;
    }

    if (aRequest.get_server_protocol() != server::kIdentifier)
    {
        response.set_success(false);
        response.set_error("Invalid protocol version!");

        spdlog::warn("Connection attempt with server identifier {:x}, expected {:x}", aRequest.get_client_protocol(), server::kIdentifier);
        Send(aRequest.ConnectionId, response);
        Kick(aRequest.ConnectionId);
        return;
    }

    spdlog::info("Authorize connection from {} with token {}", aRequest.get_username(), aRequest.get_token());

    GetWorld()->get_mut<PlayerManager>()->Create(aRequest.ConnectionId, aRequest.get_username());

    response.set_success(true);

    server::Settings settings;
    settings.set_update_rate(m_config.UpdateRate);
    response.set_settings(settings);

    server::RpcDefinitions definitions;

    const auto* pRpc = static_cast<RpcScriptInstance*>(IRpc::Get());
    pRpc->Serialize(definitions);

    if (!Send(aRequest.ConnectionId, definitions))
        spdlog::error("Failed to send message to {:x}", aRequest.ConnectionId);

    if (!Send(aRequest.ConnectionId, response))
        spdlog::error("Failed to send message to {:x}", aRequest.ConnectionId);
}

ScratchAllocator& GameServer::GetScratch()
{
    thread_local ScratchAllocator s_allocator{1 << 19};
    return s_allocator;
}
