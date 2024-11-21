#pragma once

#include "Core/Buffer.h"
#include "Core/ScratchAllocator.h"
#include "Core/Foundation/Feature.hpp"
#include "Network/Client.h"
#include "Network/Packet.h"
#include "Rpc/RpcService.h"

template <typename T> struct DebugType;

template <typename T>
concept NetworkMessage = requires(T a, Buffer::Writer writer, Buffer::Reader reader)
{
    {
        a.serialize(writer)
    } -> std::convertible_to<bool>;
    {
        a.deserialize(reader)
    } -> std::convertible_to<bool>;
};

struct NetworkService final
    : Client
    , Core::Feature
{
    NetworkService();
    ~NetworkService() override;

    template <NetworkMessage T>
    bool Send(const T& acMessage);

    template <NetworkMessage T>
    auto GetSink() noexcept { return m_dispatcher.sink<PacketEvent<T>>(); }

    template <auto Func, typename... T>
    auto RegisterHandler(T&&... args) noexcept
    {
        using MessageType = typename std::remove_cv_t<std::remove_reference_t < typename std::tuple_element<0, typename details::signature<decltype(Func)>::type>::type >>::Type;

        static_assert(NetworkMessage<MessageType>, "Handler should take a NetworkMessage as first parameter!");

        return m_dispatcher.sink<PacketEvent<MessageType>>().connect<Func>(std::forward<T>(args)...);
    }

    entt::dispatcher& GetDispatcher() { return m_dispatcher; }
    const server::Settings& GetServerSettings() const { return m_settings; }

    TP_NOCOPYMOVE(NetworkService);

protected:
    void BindMessageHandlers();

    void OnConsume(const void* apData, uint32_t aSize) override;
    void OnConnected() override;
    void OnDisconnected(EDisconnectReason aReason) override;
    void OnUpdate() override;
    void OnGameUpdate(RED4ext::CGameApplication* apApp) override;

    void HandleAuthentication(const PacketEvent<server::AuthenticationResponse>& aResponse);

    static ScratchAllocator& GetScratch();

private:

    std::chrono::time_point<std::chrono::steady_clock> m_lastUpdate;
    bool m_ready = false;
    bool m_authenticated = false;
    bool m_isPaused = false;
    entt::dispatcher m_dispatcher;
    uint64_t m_lastCharacterUpdate{};
    server::Settings m_settings;
};

template <NetworkMessage T>
bool NetworkService::Send(const T& acMessage)
{
    ScopedResetAllocator _{GetScratch()};

    Buffer buffer(1 << 18);
    Buffer::Writer writer(&buffer);
    writer.WriteBits(0, 8); // Skip the first byte as it is used by packet

    client::Serializer::Process(writer, acMessage);

    PacketView packet(reinterpret_cast<char*>(buffer.GetWriteData()), (uint32_t)writer.Size());
    Client::Send(&packet, T::kReliable ? kReliable : kUnreliable);

    return true;
}
