#include "RpcValidator.h"

#include <xbyak/xbyak.h>

#include "RpcGenerator.h"
#include "App/World/RpcDefinition.h"
#include "Core/Hooking/HookingAgent.hpp"
#include "Game/ScriptValidation.h"

bool ValidateType(const Red::CBaseRTTIType* pType, Red::CClass* klass)
{
    static auto s_floatType = Red::ResolveType<float>()->GetName();
    static auto s_int32Type = Red::ResolveType<int32_t>()->GetName();
    static auto s_uint32Type = Red::ResolveType<uint32_t>()->GetName();
    static auto s_int64Type = Red::ResolveType<int64_t>()->GetName();
    static auto s_uint64Type = Red::ResolveType<uint64_t>()->GetName();
    static auto s_nameType = Red::ResolveType<Red::CName>()->GetName();
    static auto s_doubleType = Red::ResolveType<double>()->GetName();
    static auto s_stringType = Red::ResolveType<Red::CString>()->GetName();
    static auto s_boolType = Red::ResolveType<bool>()->GetName();
    static auto s_tweakdbId = Red::ResolveType<Red::TweakDBID>()->GetName();

    if (pType == nullptr)
        return true;

    if (pType->GetName() == s_floatType || pType->GetName() == s_int32Type || pType->GetName() == s_uint32Type || pType->GetName() == s_int64Type ||
        pType->GetName() == s_uint64Type || pType->GetName() == s_nameType || pType->GetName() == s_doubleType || pType->GetName() == s_stringType ||
        pType->GetName() == s_boolType || pType->GetName() == s_tweakdbId)
        return true;

    if (pType->GetType() == Red::ERTTIType::Class)
    {
        auto* pClass = (Red::CClass*)pType;
        if (!ValidateType(pClass->parent, klass))
            return false;

        Red::DynArray<Red::CProperty*> props;
        pClass->GetProperties(props);

        for (const auto prop : props)
        {
            if (!ValidateType(prop->type, pClass))
                return false;
        }

        return true;
    }

    if (pType->GetType() == Red::ERTTIType::Array || pType->GetType() == Red::ERTTIType::FixedArray || pType->GetType() == Red::ERTTIType::NativeArray || pType->GetType() == Red::ERTTIType::StaticArray)
    {
        auto* pClass = (Red::CRTTIBaseArrayType*)pType;
        if (!ValidateType(pClass->innerType, klass))
            return false;

        return true;
    }

    return false;
}

bool ValidateType(Set<RED4ext::CName>& knownTypes, Game::ScriptType* type, Game::ScriptClass* klass, Game::ScriptFunction* func, Game::ScriptReport* apReporter)
{
    if (type == nullptr)
        return true;

    if (type->metaType == Game::EScriptType::ScriptRef ||
        type->metaType == Game::EScriptType::StrongHandle ||
        type->metaType == Game::EScriptType::WeakHandle)
    {
        auto res = std::format(
            "CyberpunkMP: RpcDefinition '{}' function '{}' type '{}' is a handle which is not supported.", klass->name.ToString(), func->name.ToString(),
            type->name.ToString());
        Game::StringView v;
        v.length = res.size();
        v.str = res.c_str();

        apReporter->AddValidationError(v);
        return false;
    }

    if (type->metaType == Game::EScriptType::DynArray ||
        type->metaType == Game::EScriptType::StaticArray)
    {
        return ValidateType(knownTypes, static_cast<Game::ScriptType*>(type->innerType), klass, func, apReporter);    
    }

    //if (type->metaType == Game::EScriptType::Primitive) -> this is always valid
    if (type->metaType == Game::EScriptType::Class)
    {
        if (const auto loc = knownTypes.find(type->name); loc != std::end(knownTypes))
            return true;

        const auto pClass = reinterpret_cast<Game::ScriptClass*>(type->innerType);
        if (!ValidateType(knownTypes, reinterpret_cast<Game::ScriptType*>(pClass->parent), klass, func, apReporter))
            return false;

        auto* pBaseClass = RED4ext::CRTTISystem::Get()->GetClassByScriptName(type->innerType->name);
        if (!pBaseClass)
            return !pClass->flags.isNative;

        for(const auto pProp : pClass->properties)
        {
            const auto* pBaseProp = pBaseClass->GetProperty(pProp->name);
            if (!pBaseProp)
                return false;

            if (!ValidateType(pBaseProp->type, pBaseClass))
                return false;
        }

        knownTypes.insert(type->name);
    }

    return true;
}

bool ValidateFunction(Game::ScriptClass* klass, Game::ScriptFunction* func, Game::ScriptReport* apReporter)
{
    if (!func->flags.isNative)
        return false;

    Set<RED4ext::CName> knownTypes;

    for (const auto param : func->params)
    {
        if (param->flags.isOut || param->flags.isOptional)
        {
            auto res = std::format(
                "CyberpunkMP: RpcDefinition '{}' function '{}' parameter '{}' is out or optional which is invalid.", klass->name.ToString(), func->name.ToString(),
                param->name.ToString());
            Game::StringView v;
            v.length = res.size();
            v.str = res.c_str();

            apReporter->AddValidationError(v);
            return false;
        }

        if (!ValidateType(knownTypes, param->type, klass, func, apReporter))
            return false;
    }

    return true;
}

bool (*RealScriptValidator_Validate)(void* apThis, Game::ScriptBundle* apBundler, Game::ScriptReport* apReporter);
bool HookScriptValidator_Validate(void* apThis, Game::ScriptBundle* apBundler, Game::ScriptReport* apReporter)
{
    apReporter->fillErrors = true;

    constexpr Red::CName parentName("CyberpunkMP.ServerRpc");
    for (const auto klass : apBundler->classes)
    {
        std::string name = klass->name.ToString();
        if (klass->parent && klass->parent->name == parentName)
        {
            const auto pClass = Red::GetClass(klass->name);
            if (!pClass)
            {
                using Descriptor = Red::ClassDescriptorDefaultImpl<RpcMock>;
                const auto pDesc = new Descriptor();
                pDesc->name = klass->name;
                pDesc->flags.isNative = 1;
                pDesc->flags.isScriptedClass = 1;
                pDesc->SetParent<ServerRpc>();

                for (const auto func : klass->functions)
                {
                    if (!ValidateFunction(klass, func, apReporter))
                        continue;

                    RpcGenerator::CreateFunction(pDesc, klass, func);
                }

                auto* rtti = Red::CRTTISystem::Get();
                rtti->RegisterType(pDesc);
            }
        }
    }

    return RealScriptValidator_Validate(apThis, apBundler, apReporter);
}

constexpr auto ScriptValidator_Validate = Core::RawFunc<
    /* hash = */ 898639042,
    /* type = */ decltype(RealScriptValidator_Validate)>();

void RpcValidator::InternalValidate()
{
    for(auto func : RpcGenerator::GetFunctions())
    {
        for (const auto param : func->params)
        {
            if (!ValidateType(param->type, func->parent))
            {
                spdlog::error(
                    "Function {} in Class {} uses {} for arg {} which is invalid!", func->fullName.ToString(), func->parent->name.ToString(), param->type->GetName().ToString(),
                    param->name.ToString());
            }
        }
    }

    auto* pParent = Red::GetClass("ClientRpc");
    RED4ext::CRTTISystem::Get()->types.for_each(
        [pParent](const Red::CName& name, Red::CBaseRTTIType* type)
        {
            if(type->GetType() == Red::ERTTIType::Class)
            {
                const auto* pClass = static_cast<Red::CClass*>(type);
                if (pClass->IsA(pParent))
                {
                    for (const auto func : pClass->funcs)
                    {
                        for (const auto param : func->params)
                        {
                            if (!ValidateType(param->type, func->parent))
                            {
                                spdlog::error(
                                    "Function {} in Class {} uses {} for arg {} which is invalid!", func->fullName.ToString(), func->parent->name.ToString(),
                                    param->type->GetName().ToString(), param->name.ToString());
                            }
                        }
                    }
                }
            }
        });
}

void RpcValidator::Attach()
{
    Hook<ScriptValidator_Validate>(&HookScriptValidator_Validate, &RealScriptValidator_Validate);
}
