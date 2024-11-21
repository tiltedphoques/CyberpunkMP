#pragma once

#include <string>
#include "steam/steamnetworkingsockets.h"
#include "SteamInterface.h"
#include "SynchronizedClock.h"


struct Packet;
struct Client
{
    enum EDisconnectReason
    {
        kTimeout,
        kLocalProblem,
        kKicked,
        kCannotResolve,
        kAborted,
        kNormal
    };

    struct Statistics
    {
        uint32_t SentBytes{};
        uint32_t RecvBytes{};
        uint32_t UncompressedSentBytes{};
        uint32_t UncompressedRecvBytes{};
    };

    Client(uint64_t aClientIdentifier, uint64_t aServerIdentifier) noexcept;
    virtual ~Client();

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(Client&&) noexcept;
    Client& operator=(Client&&) noexcept;

    bool Connect(const SteamNetworkingIPAddr& acEndpoint) noexcept;
    bool Connect(const std::string& acEndpoint) noexcept;
    bool ConnectByIp(const std::string& acEndpoint) noexcept;
    void Close() noexcept;

    void Update() noexcept;

    virtual void OnConsume(const void* apData, uint32_t aSize) = 0;
    virtual void OnConnected() = 0;
    virtual void OnDisconnected(EDisconnectReason aReason) = 0;
    virtual void OnUpdate() = 0;

    void Send(Packet* apPacket, EPacketFlags acPacketFlags = kReliable) const noexcept;

    [[nodiscard]] bool IsConnected() const noexcept;
    [[nodiscard]] SteamNetConnectionRealTimeStatus_t GetConnectionStatus() const noexcept;
    [[nodiscard]] Statistics GetStatistics() const noexcept;
    [[nodiscard]] const SynchronizedClock& GetClock() const noexcept;

private:

    static void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* apInfo);
    static void UVGetAddrInfoCallback(void* apHandle, int aStatus, void* apResult);
    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* apInfo);

    void HandleMessage(const void* apData, uint32_t aSize) noexcept;
    void HandleServerTime(const void* apData, uint32_t aSize) noexcept;
    void HandleCompressedPayload(const void* apData, uint32_t aSize) noexcept;

    void SendHandshake() const noexcept;

    HSteamNetConnection m_connection;
    ISteamNetworkingSockets* m_pInterface;
    SynchronizedClock m_clock;
    uint64_t m_lastStatisticsPoint{};
    mutable Statistics m_currentFrame{};
    Statistics m_previousFrame{};
    void* m_pLoop{};
    void* m_pHandle{};
    uint64_t m_clientIdentifier;
    uint64_t m_serverIdentifier;
};
