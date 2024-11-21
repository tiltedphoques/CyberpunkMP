#include "RpcGenerator.h"

#include <fstream>
#include <xbyak/xbyak.h>

#include "App/Network/NetworkService.h"
#include "Core/Container/Container.hpp"
#include "Game/ScriptValidation.h"

#include "RpcPack.h"

template <class T> T GetParameter(Red::CStackFrame* aFrame)
{
    T var;
    Red::GetParameter(aFrame, &var);
    return var;
}

void RunRpcFunction(void* aContext, Red::CStackFrame* aFrame, void* aRet, Red::CBaseRTTIType* aRetType, const RpcContext* apContext)
{
    const auto pRpcService = Core::Container::Get<RpcService>();

    client::RpcCall call;

    auto id = pRpcService->GetRpcId(apContext->klass, apContext->function);
    if (!id.has_value())
    {
        spdlog::error("Could not find Rpc id for class {:X}, function {:X}", apContext->klass.hash, apContext->function.hash);
        return;
    }

    call.set_id(id.value());

    Buffer buff(1 << 16);
    Buffer::Writer writer(&buff);

    for (const auto& pArg : apContext->args)
    {
        const auto* pType = pArg.pack->m_pRtti;
        auto* pTypeAllocator = pType->GetAllocator();

        auto* pInstance = pTypeAllocator->AllocAligned(pType->GetSize(), pType->GetAlignment()).memory;
        std::memset(pInstance, 0, pType->GetSize());
        pType->Construct(pInstance);

        aFrame->currentParam++;
        aFrame->data = nullptr;
        aFrame->dataType = nullptr;
        const auto opcode = *aFrame->code++;
        RED4ext::OpcodeHandlers::Run(opcode, aFrame->context, aFrame, pInstance, nullptr);

        pArg.pack->Write(pInstance, writer);

        pType->Destruct(pInstance);
        pTypeAllocator->Free(pInstance);
    }

    Vector<uint8_t> args(buff.GetData(), buff.GetData() + writer.Size());
    call.set_arguments(std::move(args));

    aFrame->code++; // skip ParamEnd
    aFrame->currentParam -= apContext->args.size();

    const auto pNetworkService = Core::Container::Get<NetworkService>();
    pNetworkService->Send(call);
}

namespace
{
struct RpcCodegen : Xbyak::CodeGenerator
{
    RpcCodegen()
    {
        const uintptr_t func = reinterpret_cast<uintptr_t>(&RunRpcFunction);
        const uintptr_t c = reinterpret_cast<uintptr_t>(&ctx);

        sub(Xbyak::util::rsp, 56);
        mov(rax, c);
        mov(ptr[rsp + 32], rax);
        mov(rax, func);
        call(rax);
        add(rsp, 56);
        ret();
    }

    RpcContext ctx;
};

std::list<RpcCodegen> s_codegen;
}

void CollectTypes(Set<Red::CClass*>& classes, Set<Red::CEnum*>& enums, const Red::CBaseRTTIType* pType)
{
    if (pType == nullptr)
        return;

    if (pType->GetType() == Red::ERTTIType::Class)
    {
        auto* pClass = (Red::CClass*)pType;

        classes.insert(pClass);

        if (pClass->parent)
            CollectTypes(classes, enums, pClass->parent);

        Red::DynArray<Red::CProperty*> props;
        pClass->GetProperties(props);

        for (const auto prop : props)
        {
            CollectTypes(classes, enums, prop->type);
        }
    }
    else if (pType->GetType() == Red::ERTTIType::Array)
    {
        const auto* pClass = static_cast<const Red::CRTTIBaseArrayType*>(pType);
        CollectTypes(classes, enums, pClass->innerType);
    }
    else if (pType->GetType() == Red::ERTTIType::Enum)
    {
        auto* pEnum = (Red::CEnum*)pType;
        enums.insert(pEnum);
    }
}

std::string SanitizeName(const std::string& name)
{
    if (const auto pos = name.find_first_of('.'); pos != std::string::npos)
        return name.substr(pos + 1);
    return name;
}

struct FunctionLibrary
{
    Vector<RED4ext::CClassFunction*> functions;
};
using FunctionLibMap = Map<const Red::CClass*, FunctionLibrary>;

namespace 
{
Vector<RpcHandler> s_clientContext;
}

void GenerateClientReaders(FunctionLibMap& map)
{
    for (const auto& [functions] : map | std::views::values)
    {
        for (const auto func : functions)
        {
            auto& h = s_clientContext.emplace_back();
            h.context.function = func->fullName.hash;
            h.context.klass = func->parent->name.hash;
            h.function = func;
            for (const auto arg : func->params)
            {
                h.context.args.emplace_back(MakePack(arg->type));
            }
        }
    }
}

void GenerateServerWriters(FunctionLibMap& map)
{
    for (const auto& [functions] : map | std::views::values)
    {
        for (auto func : functions)
        {
            auto itor = std::ranges::find_if(
                s_codegen,
                [func](const RpcCodegen& element) { return element.ctx.function == func->fullName && element.ctx.klass == func->parent->name; });

            if (itor == std::end(s_codegen))
            {
                spdlog::error("Fatal error, trying to codegen function that doesn't exist!");
                return;
            }

            for (const auto arg : func->params)
            {
                itor->ctx.args.emplace_back(MakePack(arg->type));
            }
        }
    }
}

void RpcGenerator::GenerateHandlers()
{
    FunctionLibMap serverLibrary;
    FunctionLibMap clientLibrary;

    Set<Red::CClass*> classes;
    Set<Red::CEnum*> enums;
    for (auto func : GetFunctions())
    {
        if (auto itor = serverLibrary.find(func->parent); itor == serverLibrary.end())
            serverLibrary[func->parent] = {};
        serverLibrary[func->parent].functions.push_back(func);

        for (const auto param : func->params)
        {
            CollectTypes(classes, enums, param->type);
        }
    }

    auto pParent = Red::GetClass("ClientRpc");
    RED4ext::CRTTISystem::Get()->types.for_each(
        [&classes, &enums, &clientLibrary, pParent](const Red::CName& name, Red::CBaseRTTIType* type)
        {
            if (type->GetType() == Red::ERTTIType::Class)
            {
                const auto* pClass = static_cast<Red::CClass*>(type);
                if (pClass->IsA(pParent) && pClass->funcs.size > 0)
                {
                    clientLibrary[pClass] = {};

                    for (const auto func : pClass->funcs)
                    {
                        clientLibrary[pClass].functions.push_back(func);

                        for (const auto param : func->params)
                        {
                            CollectTypes(classes, enums, param->type);
                        }
                    }
                }
            }
        });

    GenerateClientReaders(clientLibrary);
    GenerateServerWriters(serverLibrary);
}

void RpcGenerator::CreateFunction(Descriptor* desc, const Game::ScriptClass* klass, Game::ScriptFunction* func)
{
    Red::CBaseFunction::Flags flags{};
    flags.isNative = 1;
    flags.isStatic = 1;

    auto& codegen = s_codegen.emplace_back();

    codegen.ctx.klass = klass->name;
    codegen.ctx.function = func->name;

    const auto pFunc = Red::CClassStaticFunction::Create(desc, func->name.ToString(), func->name.ToString(), Red::ScriptingFunction_t<void*>(codegen.getCode()), flags);

    for (const auto param : func->params)
    {
        pFunc->AddParam(param->type->name, param->name.ToString(), param->flags.isOut, param->flags.isOptional);
    }

    desc->RegisterFunction(pFunc);

    s_functions.push_back(pFunc);
}

RpcHandler* RpcGenerator::GetRpcHandler(uint64_t klass, uint64_t func)
{
    auto it = std::ranges::find_if(s_clientContext, [=](const RpcHandler& h) { return h.context.function == func && h.context.klass == klass; });
    if (it != std::end(s_clientContext))
        return &*it;

    return nullptr;
}
