#include "RpcService.h"

#include "RpcValidator.h"
#include "App/Network/NetworkService.h"
#include <RED4ext/Scripting/Natives/GameTime.hpp>

#include "RpcGenerator.h"
#include "RpcPack.h"
#include "App/Settings.h"


RpcService::RpcService(RED4ext::PluginHandle aPlugin, const RED4ext::Sdk* aSdk)
{
    RED4ext::GameState state{nullptr, nullptr, nullptr};
    state.OnExit = &PrepareRpc;

    aSdk->gameStates->Add(aPlugin, RED4ext::EGameStateType::Initialization, &state);
}

RpcService::~RpcService()
{
    
}

void RpcService::OnInitialize()
{
    RpcValidator::Attach();

    const auto pNetworkService = Core::Container::Get<NetworkService>();
    pNetworkService->RegisterHandler<&RpcService::HandleRpc>(this);
    pNetworkService->RegisterHandler<&RpcService::HandleRpcDefinitions>(this);
}

void RpcService::OnShutdown()
{
}

std::optional<uint32_t> RpcService::GetRpcId(uint64_t aKlass, uint64_t aFunction) const
{
    const auto itor = m_serverRpcs.find(RpcId{aKlass, aFunction});
    if (itor != std::end(m_serverRpcs))
        return itor->second;

    return std::nullopt;
}

bool RpcService::PrepareRpc(RED4ext::CGameApplication* aApp)
{
    RpcValidator::InternalValidate();
    RpcGenerator::GenerateHandlers();
    RpcGenerator::DumpCsharp();

    if (Settings::Get().RpcOnly)
    {
        ExitProcess(0);
    }

    return true;
}

void RpcService::HandleRpc(const PacketEvent<server::RpcCall>& aMessage)
{
    if (!Call(aMessage))
        spdlog::error("Rpc failed");
}

void RpcService::HandleRpcDefinitions(const PacketEvent<server::RpcDefinitions>& aMessage)
{
    m_clientRpcs.resize(aMessage.get_client_definitions().size());

    for (auto& rpc : aMessage.get_client_definitions())
    {
        const RpcId id{rpc.get_klass(), rpc.get_function()};
        m_clientRpcs[rpc.get_id()] = {id, RpcGenerator::GetRpcHandler(id.Klass, id.Function)};
    }

    for (auto& rpc : aMessage.get_server_definitions())
    {
        m_serverRpcs[RpcId{rpc.get_klass(), rpc.get_function()}] = rpc.get_id();
    }
}

bool RpcService::Call(const server::RpcCall& aMessage) const
{
    auto id = aMessage.get_id();

    if (id >= m_clientRpcs.size())
    {
        spdlog::error("Failed to retrieve Rpc with id {:X}", id);
        return false;
    }

    const auto& rpc = m_clientRpcs[id];

    const auto* pContext = rpc.Handler;
    if (rpc.Id.Klass != 0 && !pContext)
        return false;

    const auto pFunc = pContext->function;
    const auto combinedArgCount = pFunc->params.size;

    static auto* cScriptContext = static_cast<RED4ext::IScriptable*>(Red::GetClass("entEntity")->CreateInstance());

    Red::CStack stack(cScriptContext);
    Red::StackArgs_t args;

    if (combinedArgCount > 0)
    {
        ViewBuffer buffer(const_cast<uint8_t*>(aMessage.get_arguments().data()), aMessage.get_arguments().size());
        Buffer::Reader reader(&buffer);
        for (auto i = 0; i < combinedArgCount; ++i)
        {
            auto* pType = pFunc->params[i]->type;
            const auto* pTypeAllocator = pType->GetAllocator();

            auto* pInstance = pTypeAllocator->AllocAligned(pType->GetSize(), pType->GetAlignment()).memory;
            std::memset(pInstance, 0, pType->GetSize());
            pType->Construct(pInstance);

            auto& arg = args.emplace_back();
            arg.value = pInstance;
            arg.type = pType;

            pContext->context.args[i].pack->Read(args.back().value, reader);
        }

        stack.args = args.data();
        stack.argsCount = pFunc->params.size;
    }

    Red::Detail::CallFunctionWithStack(nullptr, pFunc, stack);

    for (const auto& arg : args)
    {
        const auto* pType = arg.type;
        const auto* pTypeAllocator = pType->GetAllocator();

        pType->Destruct(arg.value);
        pTypeAllocator->Free(arg.value);
    }

    return true;
}

