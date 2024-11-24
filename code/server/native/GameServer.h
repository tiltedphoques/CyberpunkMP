#pragma once

#include "System/Path.h"
#include "System/Log.h"
#include "Config.h"
#include "Game/World.h"

template <typename T>
concept NetworkMessage = requires(T a, Buffer::Writer writer, Buffer::Reader reader) {
    {
        a.serialize(writer)
    } -> std::convertible_to<bool>;
    {
        a.deserialize(reader)
    } -> std::convertible_to<bool>;
};


struct GameServer final : Server
{
    TP_NOCOPYMOVE(GameServer);

    GameServer();
    ~GameServer() override;

    void Kill();
    void Run();

    template <NetworkMessage T>
    bool Send(ConnectionId aConnectionId, const T& acMessage) const;

    template<NetworkMessage T>
    auto GetSink() noexcept { return m_dispatcher.sink<PacketEvent<T>>(); }

    template <auto Func, typename... T> auto RegisterHandler(T&&... args) noexcept
    {
        using MessageType = typename std::remove_cv_t<std::remove_reference_t<typename std::tuple_element<0, typename details::signature<decltype(Func)>::type>::type>>::Type;

        static_assert(NetworkMessage<MessageType>, "Handler should take a NetworkMessage as first parameter!");

        return m_dispatcher.sink<PacketEvent<MessageType>>().template connect<Func>(std::forward<T>(args)...);
    }

    gsl::not_null<const Config*> GetConfig() const noexcept { return &m_config; }
    gsl::not_null<World*> GetWorld() noexcept { return m_pWorld.get(); }
    gsl::not_null<TaskQueue*> GetTaskQueue() noexcept { return &m_tasks; }
    gsl::not_null<Log*> GetLog() noexcept { return &m_log; }

protected:
    void OnUpdate() override;
    void OnConsume(const void* apData, uint32_t aSize, ConnectionId aConnectionId) override;
    void OnConnection(ConnectionId aHandle) override;
    void OnDisconnection(ConnectionId aConnectionId, EDisconnectReason aReason) override;

    void HandleAuthentication(const PacketEvent<client::AuthenticationRequest>& aRequest);

    static ScratchAllocator& GetScratch();

private:

    void FetchServerEntitlements();

    Path m_path;
    Log m_log;
    UniquePtr<World> m_pWorld;
    TaskQueue m_tasks;
    bool m_run = true;
    std::chrono::steady_clock::time_point m_lastUpdate;
    entt::dispatcher m_dispatcher;

    Config m_config;
};

template <NetworkMessage T>
bool GameServer::Send(ConnectionId aConnectionId, const T& acMessage) const
{
    ScopedResetAllocator _{GetScratch()};

    Buffer buffer(1 << 18);
    Buffer::Writer writer(&buffer);
    writer.WriteBits(0, 8); // Skip the first byte as it is used by packet

    server::Serializer::Process(writer, acMessage);

    PacketView packet(reinterpret_cast<char*>(buffer.GetWriteData()), (uint32_t)writer.Size());
    Server::Send(aConnectionId, &packet, T::kReliable ? kReliable : kUnreliable);

    return true;
}


extern GameServer* GServer;
