#include "RpcGenerator.h"

#include <fstream>
#include <RED4ext/Scripting/Natives/GameTime.hpp>

#include "App/Settings.h"


void CollectTypes(Set<const Red::CClass*>& classTypes, Set<const Red::CEnum*>& enumTypes, const Red::CBaseRTTIType* pType)
{
    if (pType == nullptr)
        return;

    if (pType->GetType() == Red::ERTTIType::Class)
    {
        auto* pClass = (Red::CClass*)pType;

        classTypes.insert(pClass);

        if (pClass->parent)
            CollectTypes(classTypes, enumTypes, pClass->parent);

        Red::DynArray<Red::CProperty*> props;
        pClass->GetProperties(props);

        for (const auto prop : props)
        {
            CollectTypes(classTypes, enumTypes, prop->type);
        }
    }
    else if (pType->GetType() == Red::ERTTIType::Array)
    {
        const auto* pClass = static_cast<const Red::CRTTIBaseArrayType*>(pType);
        CollectTypes(classTypes, enumTypes, pClass->innerType);
    }
    else if (pType->GetType() == Red::ERTTIType::Enum)
    {
        auto* pEnum = static_cast<const Red::CEnum*>(pType);
        enumTypes.insert(pEnum);
    }
}

std::string CsharpSanitizeName(const std::string& name)
{
    if (auto pos = name.find_first_of('.'); pos != std::string::npos)
        return name.substr(pos + 1);
    return name;
}

std::string GetTypeName(Red::CBaseRTTIType* type, bool append = true)
{
    static auto s_boolType = Red::ResolveType<bool>();
    static auto s_floatType = Red::ResolveType<float>();
    static auto s_int8Type = Red::ResolveType<int8_t>();
    static auto s_uint8Type = Red::ResolveType<uint8_t>();
    static auto s_int16Type = Red::ResolveType<int16_t>();
    static auto s_uint16Type = Red::ResolveType<uint16_t>();
    static auto s_int32Type = Red::ResolveType<int32_t>();
    static auto s_uint32Type = Red::ResolveType<uint32_t>();
    static auto s_int64Type = Red::ResolveType<int64_t>();
    static auto s_uint64Type = Red::ResolveType<uint64_t>();
    static auto s_nameType = Red::ResolveType<Red::CName>();
    static auto s_doubleType = Red::ResolveType<double>();
    static auto s_stringType = Red::ResolveType<Red::CString>();
    static auto s_tdbid = Red::ResolveType<Red::TweakDBID>();

    if (type == s_floatType)
        return "float";
    if (type == s_boolType)
        return "bool";
    if (type == s_int8Type)
        return "sbyte";
    if (type == s_uint8Type)
        return "byte";
    if (type == s_int16Type)
        return "short";
    if (type == s_uint16Type)
        return "ushort";
    if (type == s_int32Type)
        return "int";
    if (type == s_uint32Type)
        return "uint";
    if (type == s_int64Type)
        return "long";
    if (type == s_uint64Type)
        return "ulong";
    if (type == s_nameType)
        return "CName";
    if (type == s_tdbid)
        return "TweakDBID";
    if (type == s_doubleType)
        return "double";
    if (type == s_stringType)
        return "string";

    if (type->GetType() == Red::ERTTIType::Class)
    {
        const auto* pClass = static_cast<Red::CClass*>(type);
        return "Cyberpunk.Rpc." + CsharpSanitizeName(pClass->GetName().ToString());
    }
    if (type->GetType() == Red::ERTTIType::Array )
    {
        const auto* pClass = static_cast<Red::CRTTIBaseArrayType*>(type);
        auto name = "Cyberpunk.Rpc." + CsharpSanitizeName(pClass->innerType->GetName().ToString());
        if (append)
            name += "[]";
        return name;
    }
    if (type->GetType() == Red::ERTTIType::Enum)
    {
        const auto* pEnum = static_cast<Red::CEnum*>(type);
        return "Cyberpunk.Rpc." + CsharpSanitizeName(pEnum->GetName().ToString());
    }

    return "";
}

struct FunctionLibrary
{
    Vector<RED4ext::CClassFunction*> functions;
};
using FunctionLibMap = Map<const Red::CClass*, FunctionLibrary>;

void EmitRead(const char* name, Red::CBaseRTTIType* type, std::ostream& o, bool declare = true)
{
    static auto s_boolType = Red::ResolveType<bool>();
    static auto s_floatType = Red::ResolveType<float>();
    static auto s_int8Type = Red::ResolveType<int8_t>();
    static auto s_uint8Type = Red::ResolveType<uint8_t>();
    static auto s_int16Type = Red::ResolveType<int16_t>();
    static auto s_uint16Type = Red::ResolveType<uint16_t>();
    static auto s_int32Type = Red::ResolveType<int32_t>();
    static auto s_uint32Type = Red::ResolveType<uint32_t>();
    static auto s_int64Type = Red::ResolveType<int64_t>();
    static auto s_uint64Type = Red::ResolveType<uint64_t>();
    static auto s_nameType = Red::ResolveType<Red::CName>();
    static auto s_doubleType = Red::ResolveType<double>();
    static auto s_stringType = Red::ResolveType<Red::CString>();
    static auto s_tdbid = Red::ResolveType<Red::TweakDBID>();

    if (type->GetType() == Red::ERTTIType::Array)
    {
        const auto* pClass = static_cast<Red::CRTTIBaseArrayType*>(type);

        o << "\t\tlength = data.ReadUint16();\n";
        o << (declare ? "\t\tvar " : "\t\t") << name << " = new " << GetTypeName(pClass->innerType) << "[length];\n";
        o << "\t\tfor(uint array_idx = 0; array_idx < length; ++array_idx)\n\t\t{\n";
        o << "\t";
        EmitRead("_tmp_val", pClass->innerType, o);
        o << "\t\t\t" << name << "[array_idx] = _tmp_val;\n";
        o << "\t\t}\n";
        return;
    }

    o << "\t\t";
    if (type == s_floatType)
        o << (declare ? "var " : "") << name << " = data.ReadFloat();\n";
    else if (type == s_boolType)
        o << (declare ? "var " : "") << name << " = data.ReadBool();\n";
    else if (type == s_int8Type)
        o << (declare ? "var " : "") << name << " = data.ReadInt8();\n";
    else if (type == s_uint8Type)
        o << (declare ? "var " : "") << name << " = data.ReadUint8();\n";
    else if (type == s_int16Type)
        o << (declare ? "var " : "") << name << " = data.ReadInt16();\n";
    else if (type == s_uint16Type)
        o << (declare ? "var " : "") << name << " = data.ReadUint16();\n";
    else if (type == s_int32Type)
        o << (declare ? "var " : "") << name << " = data.ReadInt32();\n";
    else if (type == s_uint32Type)
        o << (declare ? "var " : "") << name << " = data.ReadUint32();\n";
    else if (type == s_int64Type)
        o << (declare ? "var " : "") << name << " = data.ReadInt64();\n";
    else if (type == s_uint64Type)
        o << (declare ? "var " : "") << name << " = data.ReadUint64();\n";
    else if (type == s_nameType)
        o << (declare ? "var " : "") << name << " = new CName(data.ReadUint64());\n";
    else if (type == s_tdbid)
        o << (declare ? "var " : "") << name << " = new TweakDBID(data.ReadUint32(), data.ReadUint8());\n";
    else if (type == s_doubleType)
        o << (declare ? "var " : "") << name << " = data.ReadDouble();\n";
    else if (type == s_stringType)
        o << (declare ? "var " : "") << name << " = data.ReadString();\n";
    else if (type->GetType() == Red::ERTTIType::Enum)
        o << (declare ? "var " : "") << name << " = (" << GetTypeName(type) << ")data.ReadVarInt();\n";
    else
        o << (declare ? "var " : "") << name << " = new " << GetTypeName(type) << "(data);\n";
}

void EmitWrite(const char* name, Red::CBaseRTTIType* type, std::ostream& o)
{
    static auto s_boolType = Red::ResolveType<bool>();
    static auto s_floatType = Red::ResolveType<float>();
    static auto s_int8Type = Red::ResolveType<int8_t>();
    static auto s_uint8Type = Red::ResolveType<uint8_t>();
    static auto s_int16Type = Red::ResolveType<int16_t>();
    static auto s_uint16Type = Red::ResolveType<uint16_t>();
    static auto s_int32Type = Red::ResolveType<int32_t>();
    static auto s_uint32Type = Red::ResolveType<uint32_t>();
    static auto s_int64Type = Red::ResolveType<int64_t>();
    static auto s_uint64Type = Red::ResolveType<uint64_t>();
    static auto s_nameType = Red::ResolveType<Red::CName>();
    static auto s_doubleType = Red::ResolveType<double>();
    static auto s_stringType = Red::ResolveType<Red::CString>();
    static auto s_tdbid = Red::ResolveType<Red::TweakDBID>();

    if (type->GetType() == Red::ERTTIType::Array)
    {
        const auto* pClass = static_cast<Red::CRTTIBaseArrayType*>(type);

        o << "\t\tlength = (ushort)" << name << ".Length;\n";
        o << "\t\tdata.WriteUint16(length);\n";
        o << "\t\tfor(uint array_idx = 0; array_idx < length; ++array_idx)\n\t\t{\n";
        o << "\t\t\tvar _tmp_val = " << name << "[array_idx];\n";
        o << "\t";
        EmitWrite("_tmp_val", pClass->innerType, o);
        o << "\t\t}\n";
        return;
    }

    o << "\t\t";
    if (type == s_floatType)
        o << "data.WriteFloat(" << name << ");\n";
    else if (type == s_boolType)
        o << "data.WriteBool(" << name << ");\n";
    else if (type == s_int8Type)
        o << "data.WriteInt8(" << name << ");\n";
    else if (type == s_uint8Type)
        o << "data.WriteUint8(" << name << ");\n";
    else if (type == s_int16Type)
        o << "data.WriteInt16(" << name << ");\n";
    else if (type == s_uint16Type)
        o << "data.WriteUint16(" << name << ");\n";
    else if (type == s_int32Type)
        o << "data.WriteInt32(" << name << ");\n";
    else if (type == s_uint32Type)
        o << "data.WriteUint32(" << name << ");\n";
    else if (type == s_int64Type)
        o << "data.WriteInt64(" << name << ");\n";
    else if (type == s_uint64Type)
        o << "data.WriteUint64(" << name << ");\n";
    else if (type == s_nameType)
        o << "data.WriteUint64(" << name << ".Hash);\n";
    else if (type == s_tdbid)
    {
        o << "data.WriteUint32(" << name << ".Hash);\n";
        o << "\t\tdata.WriteUint8(" << name << ".Length);\n";
    }
    else if (type == s_doubleType)
        o << "data.WriteDouble(" << name << ");\n";
    else if (type == s_stringType)
        o << "data.WriteString(" << name << ");\n";
    else if (type->GetType() == Red::ERTTIType::Enum)
        o << "data.WriteVarInt((ulong)" << name << ");\n";
    else
        o << name << ".Serialize(data);\n";
}

extern std::filesystem::path GCyberpunkMpLocation;

void GenerateFunctionLibraries(const FunctionLibMap& map, bool isClient, const std::filesystem::path& aRoot)
{
    for (auto& [key, val] : map)
    {
        std::ofstream o(aRoot / (isClient ? "Client" : "Server") / (CsharpSanitizeName(key->name.ToString()) + ".cs"));
        o << "// This file is automatically generated\n\n";
        o << "#pragma warning disable CS8618\n";
        o << "#pragma warning disable CS0219\n\n";
        o << "using CyberpunkSdk;\n";
        o << "using CyberpunkSdk.Types;\n\n";
        o << "using CyberpunkSdk.Systems;\n\n";
        o << "using CyberpunkSdk.Internal;\n\n";

        auto name = CsharpSanitizeName(key->name.ToString());
        std::string ns = "";
        auto pos = name.find_first_of('.');
        if (pos != std::string::npos)
        {
            ns = "." + name.substr(0, pos);
            name = name.substr(pos + 1);
        }

        if (isClient)
            o << "namespace Cyberpunk.Rpc.Client" << ns << "\n{\n";
        else
            o << "namespace Cyberpunk.Rpc.Server" << ns << "\n{\n";

        if (!isClient)
            o << "public static partial class " << name;
        else
            o << "public static class " << name;
        o << "\n";
        o << "{\n";
        for (const auto func : val.functions)
        {
            if (!isClient)
            {
                o << "\t[ServerRpc(Class = " << key->name.hash << ", Function = " << func->fullName.hash << ")]\n";
                o << "\tpublic static void " << func->shortName.ToString() << "(ulong playerId, IBuffer data)\n\t{\n";
                o << "\t\tushort length = 0;\n";
                for (const auto arg : func->params)
                {
                    EmitRead(arg->name.ToString(), arg->type, o);
                }
                o << "\n\t\t" << func->shortName.ToString() << "_Impl(playerId";
                for (const auto arg : func->params)
                {
                    o << ", ";
                    o << arg->name.ToString();
                }
                o << ");\n";
                o << "\t}\n\n";

                o << "\tpublic static partial void " << func->shortName.ToString() << "_Impl(ulong playerId";
                for (const auto arg : func->params)
                {
                    o << ", ";
                    o << GetTypeName(arg->type) << " " << arg->name.ToString();
                }
                o << ");\n\n";

            }
            else
            {
                o << "\t[ClientRpc(Class = " << key->name.hash << ", Function = " << func->fullName.hash << ")]\n";
                o << "\tpublic static void " << func->shortName.ToString() << "(ulong playerId";
                for (const auto arg : func->params)
                {
                    o << ", ";
                    o << GetTypeName(arg->type) << " " << arg->name.ToString();
                }
                o << ")\n\t{\n";

                o << "\t\tushort length = 0;\n";
                o << "\t\tvar data = IBuffer.Create();\n";
                o << "\t\ttry{\n";
                for (const auto arg : func->params)
                {
                    o << "\t";
                    EmitWrite(arg->name.ToString(), arg->type, o);
                }
                o << "\t\t\tCyberpunkSdk.Server.RpcManager.Call(playerId, " << key->name.hash << ", " << func->fullName.hash << ", data);\n";
                o << "\t\t} finally {\n";
                o << "\t\t\tIBuffer.Release(data);\n";
                o << "\t\t}\n";
                o << "\t}\n\n";
            }
        }
        o << "}" << std::endl << "}";
    }
}

void RpcGenerator::DumpCsharp()
{
    FunctionLibMap serverLibrary;
    FunctionLibMap clientLibrary;

    Set<const Red::CClass*> classes;
    Set<const Red::CEnum*> enums;
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

    auto path = GCyberpunkMpLocation / "Rpc";
    if (!Settings::Get().RpcPath.empty())
    {
        path = Settings::Get().RpcPath;
    }

    {
        std::error_code ec;
        create_directory(path, ec);
        create_directory(path / "Client", ec);
        create_directory(path / "Server", ec);

        std::ofstream o(path / "RedTypes.cs");
        o << "// This file is automatically generated\n\n";
        o << "#pragma warning disable CS8618\n";
        o << "#pragma warning disable CS0219\n\n";
        o << "using CyberpunkSdk;\n";
        o << "using CyberpunkSdk.Internal;\n";
        o << "using CyberpunkSdk.Types;\n\n";
        o << "namespace Cyberpunk.Rpc\n{\n";

        for (auto klass : enums)
        {
            auto name = CsharpSanitizeName(klass->name.ToString());
            std::string ns = "";
            auto pos = name.find_first_of('.');
            if (pos != std::string::npos)
            {
                ns = name.substr(0, pos);
                name = name.substr(pos + 1);
            }

            if (!ns.empty())
                o << "namespace " << ns << "\n{\n";

            o << "public enum " << name;
            o << "\n";
            o << "{\n";
            for (uint32_t i = 0; i < klass->hashList.size; ++i)
            {
                auto hash = klass->hashList[i];
                auto val = klass->valueList[i];
                o << "\t" << hash.ToString() << " = " << val << ",\n";
            }

            if (!ns.empty())
                o << "}\n";

            o << "}" << std::endl << std::endl;
        }

        for (auto klass : classes)
        {
            if (klass == pParent)
                continue;

            auto name = CsharpSanitizeName(klass->name.ToString());
            std::string ns = "";
            auto pos = name.find_first_of('.');
            if (pos != std::string::npos)
            {
                ns = name.substr(0, pos);
                name = name.substr(pos + 1);
            }

            if (!ns.empty())
                o << "namespace " << ns << "\n{\n";

            o << "public class " << name;
            if (klass->parent)
                o << " : " << CsharpSanitizeName(klass->parent->name.ToString());
            o << "\n";
            o << "{\n";
            RED4ext::DynArray<Red::CProperty*> props;
            const_cast<Red::CClass*>(klass)->GetProperties(props);
            for (auto prop : props)
            {
                o << "\tpublic " << GetTypeName(prop->type) << " " << prop->name.ToString() << ";\n";
            }

            o << "\n\tpublic " << name << "()\n\t{\n\t}\n";

            o << "\n\tpublic " << name << "(IBuffer data)\n\t{\n";
            o << "\t\tushort length = 0;\n";
            for (auto prop : props)
            {
                EmitRead(prop->name.ToString(), prop->type, o, false);
            }
            o << "\t}\n\n";
            o << "\n\tpublic void Serialize(IBuffer data)\n\t{\n";
            o << "\t\tushort length = 0;\n";
            for (auto prop : props)
            {
                EmitWrite(prop->name.ToString(), prop->type, o);
            }
            o << "\t}\n";

            if (!ns.empty())
                o << "}\n";

            o << "}" << std::endl << std::endl;
        }
        o << "}";
    }
    
    GenerateFunctionLibraries(clientLibrary, true, path);
    GenerateFunctionLibraries(serverLibrary, false, path);
}

