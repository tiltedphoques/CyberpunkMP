#pragma once

#include <steam/steamnetworkingsockets.h>
#include <cstdint>
#include <chrono>
#include "SteamInterface.h"
#include <Core/Stl.h>


struct Packet;
struct Server
{
    Server(uint64_t aClientIdentifier, uint64_t aServerIdentifier) noexcept;
    virtual ~Server();

    TP_NOCOPYMOVE(Server);

    bool Host(uint16_t aPort, uint32_t aTickRate, bool bEnableDualStackIP = true) noexcept;
    void Close() noexcept;

    void Update() noexcept;

    enum EDisconnectReason : int
    {
        Unknown,
        Quit,
        Kicked,
        Banned,
        BadConnection,
        TimedOut
    };

    virtual void OnUpdate() = 0;
    virtual void OnConsume(const void* apData, uint32_t aSize, ConnectionId aConnectionId) = 0;
    virtual void OnConnection(ConnectionId aHandle) = 0;
    virtual void OnDisconnection(ConnectionId aConnectionId,
                                 EDisconnectReason aReason) = 0;

    void SendToAll(Packet* apPacket, EPacketFlags aPacketFlags = kReliable) noexcept;
    void Send(ConnectionId aConnectionId, Packet* apPacket, EPacketFlags aPacketFlags = kReliable) const noexcept;
    void Kick(ConnectionId aConnectionId) noexcept;

    [[nodiscard]] uint16_t GetPort() const noexcept;
    [[nodiscard]] bool IsListening() const noexcept;
    [[nodiscard]] uint32_t GetClientCount() const noexcept;
    [[nodiscard]] uint32_t GetTickRate() const noexcept;
    [[nodiscard]] uint64_t GetTick() const noexcept;
    [[nodiscard]] SteamNetConnectionInfo_t GetConnectionInfo(ConnectionId aConnectionId) const noexcept;
    [[nodiscard]] bool IsAlive(ConnectionId aConnectionId) const noexcept;

private:

    void Remove(ConnectionId aId) noexcept;

    void HandleMessage(const void* apData, uint32_t aSize, ConnectionId aConnectionId) noexcept;
    void HandleCompressedPayload(const void* apData, uint32_t aSize, ConnectionId aConnectionId) noexcept;
    void HandleHandshake(const uint8_t* apData, uint32_t aSize, ConnectionId aConnectionId) noexcept;

    void SynchronizeClientClocks(ConnectionId aSpecificConnection = k_HSteamNetConnection_Invalid) noexcept;

    static void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* apInfo);
    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* apInfo);

    HSteamListenSocket m_listenSock;
    HSteamNetPollGroup m_pollGroup;
    ISteamNetworkingSockets* m_pInterface;

    Vector<ConnectionId> m_queuedConnections;
    Vector<ConnectionId> m_connections;

    uint32_t m_tickRate;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastClockSyncTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdateTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_currentTick;
    std::chrono::milliseconds m_timeBetweenUpdates;

    uint64_t m_clientIdentifier;
    uint64_t m_serverIdentifier;
};

