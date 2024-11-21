#include "Client.h"
#include "SteamInterface.h"
#include <steam/isteamnetworkingutils.h>
#include <cassert>
#include "Core/Allocator.h"
#include <uv.h>
#include "Packet.h"
#include <google/protobuf/stubs/port.h>
#include <snappy.h>


static thread_local Client* s_pClient = nullptr;

Client::Client(uint64_t aClientIdentifier, uint64_t aServerIdentifier) noexcept
    : m_clientIdentifier(aClientIdentifier)
    , m_serverIdentifier(aServerIdentifier)
{
    SteamInterface::Acquire();

    m_connection = k_HSteamNetConnection_Invalid;
    m_pInterface = SteamNetworkingSockets();

    m_pLoop = Allocator::GetDefault()->Allocate(sizeof(uv_loop_t));
    auto* pLoop = static_cast<uv_loop_t*>(m_pLoop);
    uv_loop_init(pLoop);
    pLoop->data = this;
}

Client::~Client()
{
    uv_loop_close(static_cast<uv_loop_t*>(m_pLoop));
    Allocator::Get()->Free(m_pLoop);
    SteamInterface::Release();
}

Client::Client(Client&& aRhs) noexcept
    : m_connection(k_HSteamNetConnection_Invalid)
    , m_pInterface(nullptr)
{
    SteamInterface::Acquire();

    this->operator=(std::move(aRhs));
}

Client& Client::operator=(Client&& aRhs) noexcept
{
    std::swap(m_connection, aRhs.m_connection);
    std::swap(m_pInterface, aRhs.m_pInterface);
    std::swap(m_clientIdentifier, aRhs.m_clientIdentifier);
    std::swap(m_serverIdentifier, aRhs.m_serverIdentifier);

    return *this;
}

void Client::SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* apInfo)
{
    if (!apInfo || apInfo->m_hConn == k_HSteamNetConnection_Invalid) [[unlikely]]
        return;

    if (s_pClient) [[likely]]
    {
        s_pClient->OnSteamNetConnectionStatusChanged(apInfo);
    }
}

bool Client::Connect(const std::string& acEndpoint) noexcept
{
    static auto GetAddrInfoCallback = [](uv_getaddrinfo_t* apHandle, int aStatus, struct addrinfo* apResult)
    {
        SteamNetworkingIPAddr remoteAddress{};
        bool valid = false;

        auto* pClient = static_cast<Client*>(apHandle->loop->data);
        pClient->m_pHandle = nullptr;
        
        if (aStatus == 0)
        {
            switch (apResult->ai_family)
            {
            case AF_INET:
            {
                const auto port = ntohs(reinterpret_cast<sockaddr_in*>(apResult->ai_addr)->sin_port);
                remoteAddress.SetIPv4(ntohl(reinterpret_cast<sockaddr_in*>(apResult->ai_addr)->sin_addr.s_addr), port);
                valid = true;
            } break;
            case AF_INET6:
            {
                const auto port = ntohs(reinterpret_cast<sockaddr_in6*>(apResult->ai_addr)->sin6_port);
                remoteAddress.SetIPv6(reinterpret_cast<sockaddr_in6*>(apResult->ai_addr)->sin6_addr.s6_addr, port);
                valid = true;
            } break;
            }
        }

        uv_freeaddrinfo(apResult);
        Allocator::GetDefault()->Free(apHandle);

        if(aStatus == UV_ECANCELED)
        {
            pClient->OnDisconnected(kAborted);
            return;
        }

        if (!valid)
        {
            pClient->OnDisconnected(kCannotResolve);
            return;
        }

        const auto cConnectResult = pClient->Connect(remoteAddress);
        if (!cConnectResult)
        {
            pClient->OnDisconnected(kLocalProblem);
        }
    };

    Close();

    const auto pos = acEndpoint.find_last_of(':');
    auto* pHandle = static_cast<uv_getaddrinfo_t*>(Allocator::GetDefault()->Allocate(sizeof(uv_getaddrinfo_t)));

    std::string endpoint = acEndpoint;
    std::string serviceName = "25681";
    if(pos != std::string::npos)
    {
        serviceName = acEndpoint.c_str() + 1 + pos;
        endpoint = endpoint.substr(0, pos);
    }

    m_pHandle = pHandle;
    uv_getaddrinfo(static_cast<uv_loop_t*>(m_pLoop), pHandle, GetAddrInfoCallback, endpoint.c_str(), serviceName.c_str(), nullptr);

    return true;
}

bool Client::Connect(const SteamNetworkingIPAddr& acEndpoint) noexcept
{
    Close();

    SteamNetworkingConfigValue_t opt = {};
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, reinterpret_cast<void*>(&SteamNetConnectionStatusChangedCallback));
    m_connection = m_pInterface->ConnectByIPAddress(acEndpoint, 1, &opt);

    return m_connection != k_HSteamNetConnection_Invalid;
}

bool Client::ConnectByIp(const std::string& acEndpoint) noexcept
{
    SteamNetworkingIPAddr remoteAddress{};
    remoteAddress.ParseString(acEndpoint.c_str());

    return Connect(remoteAddress);
}

void Client::Close() noexcept
{
    if (m_connection != k_HSteamNetConnection_Invalid)
    {
        m_pInterface->CloseConnection(m_connection, 0, nullptr, true);
        m_connection = k_HSteamNetConnection_Invalid;

        m_clock.Reset();

        OnDisconnected(kAborted);
    }

    if(m_pHandle != nullptr)
    {
        uv_cancel(static_cast<uv_req_t*>(m_pHandle));
    }
}

void Client::Update() noexcept
{
    m_clock.Update();

    if (m_clock.GetCurrentTick() - m_lastStatisticsPoint >= 1000)
    {
        m_lastStatisticsPoint = m_clock.GetCurrentTick();
        m_previousFrame = m_currentFrame;
        m_currentFrame = {};
    }

    s_pClient = this;
    m_pInterface->RunCallbacks();
    uv_run(static_cast<uv_loop_t*>(m_pLoop), UV_RUN_NOWAIT);
    s_pClient = nullptr;

    while (true)
    {
        ISteamNetworkingMessage* pIncomingMsg = nullptr;
        const auto cMessageCount = m_pInterface->ReceiveMessagesOnConnection(m_connection, &pIncomingMsg, 1);
        if (cMessageCount <= 0 || pIncomingMsg == nullptr)
        {
            // TODO: Handle error when messageCount < 0
            break;
        }

        m_currentFrame.RecvBytes += pIncomingMsg->GetSize();
        m_currentFrame.UncompressedRecvBytes += pIncomingMsg->GetSize();

        HandleMessage(pIncomingMsg->GetData(), pIncomingMsg->GetSize());

        pIncomingMsg->Release();
    }

    OnUpdate();
}

void Client::Send(Packet* apPacket, const EPacketFlags acPacketFlags) const noexcept
{
    m_currentFrame.UncompressedSentBytes += apPacket->m_size;

    if (apPacket->m_pData[0] == kPayload)
    {
        std::string data;
        snappy::Compress(apPacket->GetData(), apPacket->GetSize(), &data);

        if (data.size() < apPacket->GetSize())
        {
            apPacket->m_pData[0] = kCompressedPayload;
            std::copy(std::begin(data), std::end(data), apPacket->GetData());
            apPacket->m_size = (data.size() + 1) & 0xFFFFFFFF;
        }
    }

    m_currentFrame.SentBytes += apPacket->m_size;

    m_pInterface->SendMessageToConnection(m_connection, apPacket->m_pData, apPacket->m_size,
        acPacketFlags == kReliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable, nullptr);
}

bool Client::IsConnected() const noexcept
{
    if (m_connection != k_HSteamNetConnection_Invalid)
    {
        SteamNetConnectionInfo_t info{};
        if (m_pInterface->GetConnectionInfo(m_connection, &info))
        {
            if(info.m_eState == k_ESteamNetworkingConnectionState_Connected)
            {
                return GetClock().IsSynchronized();
            }
        }
    }

    return false;
}

SteamNetConnectionRealTimeStatus_t Client::GetConnectionStatus() const noexcept
{
    SteamNetConnectionRealTimeStatus_t status{};

    if (m_connection != k_HSteamNetConnection_Invalid)
    {
        m_pInterface->GetConnectionRealTimeStatus(m_connection, &status, 0, nullptr);
    }

    return status;
}

Client::Statistics Client::GetStatistics() const noexcept
{
    return m_previousFrame;
}

const SynchronizedClock& Client::GetClock() const noexcept
{
    return m_clock;
}

void Client::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* apInfo)
{
    switch (apInfo->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_None:
        break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    {
        m_pInterface->CloseConnection(m_connection, 0, nullptr, false);
        m_connection = k_HSteamNetConnection_Invalid;

        m_clock.Reset();

        if (apInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting)
        {
            OnDisconnected(kTimeout);
        }
        else if (apInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
        {
            OnDisconnected(kLocalProblem);
        }
        else
        {
            OnDisconnected(kKicked);
        }

        break;
    }
    case k_ESteamNetworkingConnectionState_Connecting:
        break;
    case k_ESteamNetworkingConnectionState_Connected:
        SendHandshake();
        break;
    default:
        break;
    }
}

void Client::HandleMessage(const void* apData, uint32_t aSize) noexcept
{
    // We handle the cases where packets target the current stack or the user stack
    if (aSize == 0)
        return;

    auto pData = static_cast<const uint8_t*>(apData);

    const auto cOpcode = pData[0];

    pData += 1;
    aSize -= 1;

    switch (cOpcode)
    {
    case kPayload:
        OnConsume(pData, aSize);
        break;
    case kServerTime:
        HandleServerTime(pData, aSize);
        break;
    case kCompressedPayload:
        HandleCompressedPayload(pData, aSize);
        break;
    default:
        assert(false);
        break;
    }
}

void Client::HandleServerTime(const void* apData, uint32_t aSize) noexcept
{
    if (aSize < 8)
        return;

    const auto cConnectionStatus = GetConnectionStatus();

    const auto cServerTime = google::protobuf::BigEndian::Load64(apData);
    const auto cWasSynchronized = GetClock().IsSynchronized();

    m_clock.Synchronize(cServerTime, cConnectionStatus.m_nPing);

    if (!cWasSynchronized)
        OnConnected();
}

void Client::HandleCompressedPayload(const void* apData, uint32_t aSize) noexcept
{
    std::string data;
    snappy::Uncompress((const char*)apData, aSize, &data);

    m_currentFrame.UncompressedRecvBytes -= aSize;
    m_currentFrame.UncompressedRecvBytes += data.size() & 0xFFFFFFFF;

    if (!data.empty()) [[likely]]
    {
        OnConsume(data.data(), data.size() & 0xFFFFFFFF);
    }
}

void Client::SendHandshake() const noexcept
{
    uint8_t data[1 + sizeof(m_clientIdentifier) + sizeof(m_serverIdentifier)];
    data[0] = kHandshake;
    google::protobuf::BigEndian::Store64(data + 1, m_clientIdentifier);
    google::protobuf::BigEndian::Store64(data + 1 + sizeof(m_clientIdentifier), m_serverIdentifier);

    m_pInterface->SendMessageToConnection(m_connection, data, std::size(data), k_nSteamNetworkingSend_Reliable, nullptr);
}

