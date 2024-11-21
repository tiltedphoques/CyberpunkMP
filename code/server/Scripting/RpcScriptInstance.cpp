#include "RpcScriptInstance.h"
#include <GameServer.h>

#include "Components/PlayerComponent.h"


RpcScriptInstance::RpcScriptInstance()
{
    GServer->RegisterHandler<&RpcScriptInstance::HandleRpcCall>(this);
}

void RpcScriptInstance::Call(uint64_t Player, uint64_t Klass, uint64_t Function, IBuffer* Arguments)
{
    const flecs::entity player(GServer->GetWorld()->get_world(), Player);

    if (!player)
        return;

    const auto pComponent = player.get<PlayerComponent>();
    if (!pComponent)
        return;

    server::RpcCall message;

    const auto itor = m_clientIds.find(RpcId{Klass, Function});
    if (itor == std::end(m_clientIds))
    {
        spdlog::error("Attempt to call client function with class {:X} and function {:X} but could not be found!", Klass, Function);
        return;
    }

    message.set_id(itor->second);

    if (Arguments)
    {
        const auto pArgs = static_cast<BufferScriptInstance*>(Arguments);
        Vector<uint8_t> args(pArgs->GetData(), pArgs->GetData() + pArgs->Size());
        message.set_arguments(std::move(args));
    }

    GServer->Send(pComponent->Connection, message);
}

uint32_t RpcScriptInstance::Count()
{
    return m_calls.size() & 0xFFFFFFFF;
}

RpcCall* RpcScriptInstance::GetRpc(uint32_t aIndex)
{
    if (aIndex < m_calls.size())
        return &m_calls[aIndex];

    return nullptr;
}

void RpcScriptInstance::Clear()
{
    m_calls.clear();
}

uint32_t RpcScriptInstance::RegisterClient(uint64_t Klass, uint64_t Function)
{
    uint32_t id = m_clientRpcs.size() & 0xFFFFFFFF;

    m_clientRpcs.emplace_back(id, Klass, Function);
    m_clientIds[RpcId{Klass, Function}] = id;

    return id;
}

uint32_t RpcScriptInstance::RegisterServer(uint64_t Klass, uint64_t Function)
{
    uint32_t id = m_serverRpcs.size() & 0xFFFFFFFF;

    m_serverRpcs.emplace_back(id, Klass, Function);

    return id;
}

void RpcScriptInstance::Serialize(server::RpcDefinitions& aDefinitions) const
{
    Vector<server::RpcDefinition> serverDefs;
    for (auto& rpc : m_serverRpcs)
    {
        server::RpcDefinition definition;
        definition.set_id(rpc.Id);
        definition.set_klass(rpc.Klass);
        definition.set_function(rpc.Function);

        serverDefs.push_back(definition);
    }

    aDefinitions.set_server_definitions(std::move(serverDefs));

    Vector<server::RpcDefinition> clientDefs;
    for (auto& rpc : m_clientRpcs)
    {
        server::RpcDefinition definition;
        definition.set_id(rpc.Id);
        definition.set_klass(rpc.Klass);
        definition.set_function(rpc.Function);

        clientDefs.push_back(definition);
    }

    aDefinitions.set_client_definitions(std::move(clientDefs));
}

void RpcScriptInstance::HandleRpcCall(const PacketEvent<client::RpcCall>& aMessage)
{
    const auto* pPlayerManager = GServer->GetWorld()->get_mut<PlayerManager>();
    if (const auto player = pPlayerManager->GetByConnectionId(aMessage.ConnectionId))
    {
        auto& call = m_calls.emplace_back(aMessage.get_id(), player.raw_id());

        call.m_buffer.Set(aMessage.get_arguments());
    }
    else
    {
        spdlog::warn("Rpc associated with unknown player {:x}", aMessage.ConnectionId);
    }
}


uint32_t RpcScriptInstance::GetId()
{
    return m_id++;
}

// Refactor this, move it somewhere else
TP_EXPORT IRpc* IRpc::Get()
{
    static RpcScriptInstance s_instance;
    return &s_instance;
}


RpcCallExtended::~RpcCallExtended()
{
}
