#include "cpp/helpers.h"
#include "Indent.h"
#include "ErrorHandler.h"

std::string GetType(const google::protobuf::FieldDescriptor* field)
{
    std::string name;
    const auto type = field->type();
    if (type == google::protobuf::FieldDescriptor::TYPE_STRING)
    {
        name = "String";
    }
    else if (type == google::protobuf::FieldDescriptor::TYPE_BYTES)
    {
        name = "Vector<uint8_t>";
    }
    else if (type == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
    {
        name = google::protobuf::compiler::cpp::ClassName(field->message_type());
        if (field->message_type()->file()->package() != field->file()->package())
            name = field->message_type()->file()->package() + "::" + name;
    }
    else if (type == google::protobuf::FieldDescriptor::TYPE_ENUM)
    {
        name = google::protobuf::compiler::cpp::ClassName(field->enum_type());
        if (field->message_type()->file()->package() != field->file()->package())
            name = field->message_type()->file()->package() + "::" + name;
    }
    else
    {
        name = google::protobuf::compiler::cpp::PrimitiveTypeName(field->cpp_type());
    }

    if (field->is_map())
    {
        return "Map<" + GetType(field->message_type()->map_key()) + ", " + GetType(field->message_type()->map_value()) + ">";
    }

    if (field->is_repeated())
    {
        return "Vector<" + name + ">";
    }

    return name;
}

std::string GetInternalType(const google::protobuf::FieldDescriptor* field)
{
    std::string name;
    const auto type = field->type();
    if (type == google::protobuf::FieldDescriptor::TYPE_STRING)
    {
        name = "String";
    }
    else if (type == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
    {
        name = google::protobuf::compiler::cpp::ClassName(field->message_type());
    }
    else if (type == google::protobuf::FieldDescriptor::TYPE_ENUM)
    {
        name = google::protobuf::compiler::cpp::ClassName(field->enum_type());
    }
    else
    {
        name = google::protobuf::compiler::cpp::PrimitiveTypeName(field->cpp_type());
    }

    if (field->is_map())
    {
        return "Map<" + GetType(field->message_type()->map_key()) + ", " + GetType(field->message_type()->map_value()) + ">";
    }

    return name;
}

struct Context
{
    struct Message
    {
        const google::protobuf::Descriptor* descriptor;
        std::vector<const google::protobuf::FieldDescriptor*> optimized_order;
        std::vector<const google::protobuf::FieldDescriptor*> optional_fields;
        std::vector<const google::protobuf::OneofDescriptor*> oneof_fields;
        bool reliable{true};
    };

    std::vector<Message> messages;
};

using namespace google::protobuf::compiler;

std::string HashProtocol(const google::protobuf::FileDescriptor* apFile, Context& ctx)
{
    std::ostringstream out;

    for (auto i = 0; i < apFile->public_dependency_count(); ++i)
    {
        auto pDep = apFile->public_dependency(i);
        auto name = cpp::StripProto(pDep->name());
        out << fmt::format(R"(dep_{}.)", name);
    }

    for (auto i = 0; i < apFile->dependency_count(); ++i)
    {
        auto pDep = apFile->dependency(i);
        auto name = cpp::StripProto(pDep->name());
        out << fmt::format(R"(dep_{}.)", name);
    }
    out << "package_" << apFile->package() << ".";

    for (auto i = 0; i < apFile->enum_type_count(); ++i)
    {
        auto pEnum = apFile->enum_type(i);
        out << "enum_" << cpp::ClassName(pEnum) << ".";
        for (auto j = 0; j < pEnum->value_count(); ++j)
        {
            auto* pValue = pEnum->value(j);
            out << "element_" << cpp::EnumValueName(pValue) << "_" << pValue->number() << ".";
        }
    }

    const Context::Message* protocol = nullptr;

    for (const auto& msg : ctx.messages)
    {
        if (cpp::ClassName(msg.descriptor) == "Protocol")
        {
            protocol = &msg;
            continue;
        }

        out << "struct_" << cpp::ClassName(msg.descriptor) << ".";

        for (auto field : msg.optimized_order)
        {
            out << "field_" << GetType(field) << "-" << cpp::FieldName(field) << "_" << field->is_optional() << ".";
        }
    }

    if (protocol)
    {
        out << "protocol.";
        for (auto& oneof : protocol->oneof_fields)
        {
            for (auto i = 0; i < oneof->field_count(); ++i)
            {
                const auto field = oneof->field(i);
                out << "field_" << i << "_" << GetType(field) << ".";
            }
        }
    }

    return out.str();
}


void GenerateProtocolHeader(const Context::Message& ctx, std::ostringstream& out)
{
    out << "struct Deserializer" << std::endl;
    out << "{" << std::endl;
    {
        Indent _{out};
        out << "[[nodiscard]] static bool Process(Buffer::Reader& reader, uint32_t aConnectionId, entt::dispatcher& dispatcher);" << std::endl;
    }
    out << "};" << std::endl << std::endl;
    out << "struct Serializer" << std::endl;
    out << "{" << std::endl;
    {
        Indent _{out};
        for (auto& oneof : ctx.oneof_fields)
        {
            for (auto i = 0; i < oneof->field_count(); ++i)
            {
                const auto field = oneof->field(i);

                out << "static void Process(Buffer::Writer& writer, const " << GetType(field) << "& msg);" << std::endl;
            }
        }
    }
    out << "};" << std::endl;
}

void GenerateProtocolSource(const Context::Message& ctx, std::ostringstream& out)
{
    for (auto& oneof : ctx.oneof_fields)
    {
        for (auto i = 0; i < oneof->field_count(); ++i)
        {
            const auto field = oneof->field(i);

            out << "void Serializer::Process(Buffer::Writer& writer, const " << GetType(field) << "& msg)" << std::endl;
            out << "{" << std::endl;
            {
                Indent _{out};
                out << "Serialization::WriteVarInt(writer, " << i << ");" << std::endl;
                out << "msg.serialize(writer);" << std::endl;
            }

            out << "}" << std::endl << std::endl;
        }
    }

    out << "bool Deserializer::Process(Buffer::Reader& reader, uint32_t aConnectionId, entt::dispatcher& dispatcher)" << std::endl;
    out << "{" << std::endl;
    {
        Indent _{out};
        out << "uint64_t opcode = Serialization::ReadVarInt(reader);" << std::endl;
        out << "switch(opcode)" << std::endl;
        out << "{" << std::endl;
        for (auto& oneof : ctx.oneof_fields)
        {
            for (auto i = 0; i < oneof->field_count(); ++i)
            {
                const auto field = oneof->field(i);
                out << "case " << i << ":" << std::endl;
                out << "{" << std::endl;
                {
                    Indent _{out};
                    out << "PacketEvent<" << GetType(field) << "> msg;" << std::endl;
                    out << "msg.ConnectionId = aConnectionId;" << std::endl;
                    out << "msg.deserialize(reader);" << std::endl;
                    out << "dispatcher.enqueue(msg);" << std::endl;
                    out << "return true;" << std::endl;
                }

                out << "}" << std::endl;
            }
        }
        out << "default: return false;" << std::endl;
        out << "}" << std::endl;
    }
    out << "}" << std::endl;
}


void GenerateHeader(std::filesystem::path aPath, const google::protobuf::FileDescriptor* apFile, Context& ctx)
{
    std::ostringstream out;

    out << "#pragma once" << std::endl << std::endl;
    for (auto i = 0; i < apFile->public_dependency_count(); ++i)
    {
        auto pDep = apFile->public_dependency(i);
        auto name = cpp::StripProto(pDep->name());
        out << fmt::format(R"(#include "{}.gen.h")", name);
    }

    for (auto i = 0; i < apFile->dependency_count(); ++i)
    {
        auto pDep = apFile->dependency(i);
        auto name = cpp::StripProto(pDep->name());
        out << fmt::format(R"(#include "{}.gen.h")", name) << std::endl;
    }
    out << std::endl << std::endl;
    out << "namespace " << apFile->package() << std::endl;
    out << "{" << std::endl;

    for (auto i = 0; i < apFile->enum_type_count(); ++i)
    {
        auto pEnum = apFile->enum_type(i);
        out << "enum " << cpp::ClassName(pEnum) << std::endl;
        out << "{" << std::endl;
        {
            Indent _(out);

            for (auto j = 0; j < pEnum->value_count(); ++j)
            {
                auto* pValue = pEnum->value(j);
                out << cpp::EnumValueName(pValue) << " = " << pValue->number() << "," << std::endl;
            }
            out << cpp::ClassName(pEnum) << "_COUNT = " << pEnum->value_count() << std::endl;
        }
        out << "};" << std::endl << std::endl;
    }

    const Context::Message* protocol = nullptr;

    for (const auto& msg : ctx.messages)
    {
        if (cpp::ClassName(msg.descriptor) == "Protocol")
        {
            protocol = &msg;
            continue;
        }

        out << "struct " << cpp::ClassName(msg.descriptor) << std::endl;
        out << "{" << std::endl;
        {
            Indent _(out);

            out << "constexpr static bool kReliable = " << std::boolalpha << msg.reliable << ";" << std::endl;
            
            out << "// Interface" << std::endl;
            out << "bool serialize(Buffer::Writer&) const;" << std::endl;
            out << "bool deserialize(Buffer::Reader&);" << std::endl;
            out << std::endl;
            Set<const google::protobuf::FieldDescriptor*> processed;
            for (auto i = 0u; i < msg.optional_fields.size(); ++i)
            {
                auto field = msg.optional_fields[i];
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                out << "bool has_" << cpp::FieldName(field) << "() const noexcept;" << std::endl;
                out << "void set_" << cpp::FieldName(field) << "(" << GetType(field) << " val) noexcept;" << std::endl;
                out << "const " << GetType(field) << "& get_" << cpp::FieldName(field) << "() const noexcept;" << std::endl;
            }

            for (auto i = 0u; i < msg.optimized_order.size(); ++i)
            {
                auto field = msg.optimized_order[i];
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                out << "void set_" << cpp::FieldName(field) << "(" << GetType(field) << " val) noexcept;" << std::endl;
                out << "const " << GetType(field) << "& get_" << cpp::FieldName(field) << "() const noexcept;" << std::endl;
            }
            out << std::endl;
            out << "private:" << std::endl;
            out << "// Fields" << std::endl;

            for (auto field : msg.optimized_order)
            {
                out << GetType(field) << " " << cpp::FieldName(field) << "{};" << std::endl;
            }

            if (!msg.optional_fields.empty())
            {
                out << "std::bitset<" << msg.optional_fields.size() << "> internal_data_bitfield_;" << std::endl;
            }
        }
        out << "};" << std::endl << std::endl;
    }

    if (protocol)
    {
        const auto str = HashProtocol(apFile, ctx);
        out << "static std::string kProtocolString = \"" << str << "\";" << std::endl;
        out << "inline constexpr size_t kIdentifier = 0x" << std::hex << FHash::FNV1a64(str.c_str()) << "ULL;" << std::endl << std::endl;
        GenerateProtocolHeader(*protocol, out);
    }

    out << "} // namespace " << apFile->package() << std::endl;

    SaveFile(aPath, out.str().c_str());
}

void GenerateUniqueDeserializer(std::ostream& out, const google::protobuf::FieldDescriptor* field, std::string name = "")
{
    if (name.empty())
        name = cpp::FieldName(field);

    out << "{" << std::endl;
    {
        Indent _(out);

        if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
        {
            out << name << ".deserialize(aReader);" << std::endl;
        }
        else if (field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
        {
            out << name << " = Serialization::ReadString(aReader);" << std::endl;
        }
        else if (field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
        {
            out << name << " = Serialization::ReadBytes(aReader);" << std::endl;
        }
        else
        {
            if (field->type() == google::protobuf::FieldDescriptor::TYPE_FIXED32 || field->type() == google::protobuf::FieldDescriptor::TYPE_SFIXED32 ||
                field->type() == google::protobuf::FieldDescriptor::TYPE_FLOAT)
            {
                out << "aReader.ReadBits(scratch, 32);" << std::endl;
                out << "*reinterpret_cast<uint32_t*>(&" << name << ") = scratch & 0xFFFFFFFF;" << std::endl;
            }
            else if (
                field->type() == google::protobuf::FieldDescriptor::TYPE_FIXED64 || field->type() == google::protobuf::FieldDescriptor::TYPE_SFIXED64 ||
                field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
            {
                out << "aReader.ReadBits(scratch, 64);" << std::endl;
                out << "*reinterpret_cast<uint64_t*>(&" << name << ") = scratch;" << std::endl;
            }
            else if (field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL)
            {
                out << name << " = Serialization::ReadBool(aReader);" << std::endl;
            }
            else if (field->type() == google::protobuf::FieldDescriptor::TYPE_ENUM)
            {
                out << name << " = static_cast<" << GetType(field) << ">(Serialization::ReadVarInt(aReader) & ~(" << cpp::PrimitiveTypeName(field->cpp_type())
                    << "{0}));" << std::endl;
            }
            else
            {
                out << name << " = Serialization::ReadVarInt(aReader) & ~(" << GetInternalType(field) << "{0});" << std::endl;
            }
        }
    }
    out << "}" << std::endl << std::endl;
}

void GenerateDeserializer(std::ostream& out, const google::protobuf::FieldDescriptor* field)
{
    out << "{" << std::endl;
    {
        Indent _(out);

        if (field->is_map())
        {
            auto itor_name = "internal_" + cpp::FieldName(field) + "_itor";

            out << "uint16_t count = Serialization::ReadVarInt(aReader) & 0xFFFF;" << std::endl;
            out << "for (auto " << itor_name << " = 0; " << itor_name << " < count; ++" << itor_name << ")" << std::endl;
            out << "{" << std::endl;
            {
                Indent _(out);
                out << GetType(field->message_type()->map_key()) << " " << itor_name << "_key;" << std::endl;
                out << GetType(field->message_type()->map_value()) << " " << itor_name << "_value;" << std::endl;
                GenerateUniqueDeserializer(out, field->message_type()->map_key(), itor_name + "_key");
                GenerateUniqueDeserializer(out, field->message_type()->map_value(), itor_name + "_value");
                out << cpp::FieldName(field) << "[" << itor_name << "_key] = std::move(" << itor_name << "_value);" << std::endl; 
            }
            out << "}" << std::endl;
        }
        else if (field->is_repeated())
        {
            auto itor_name = "internal_" + cpp::FieldName(field) + "_itor";

            out << "uint16_t count = Serialization::ReadVarInt(aReader) & 0xFFFF;" << std::endl;
            out << cpp::FieldName(field) << ".reserve(count);" << std::endl;
            out << "for (auto " << itor_name << " = 0; " << itor_name << " < count; ++" << itor_name << ")" << std::endl;
            out << "{" << std::endl;
            {
                Indent _(out);
                out << "auto& " << itor_name << "_data = " << cpp::FieldName(field) << ".emplace_back();" << std::endl;
                GenerateUniqueDeserializer(out, field, itor_name + "_data");
            }
            out << "}" << std::endl;
        }
        else
        {
            GenerateUniqueDeserializer(out, field);
        }
    }
    out << "}" << std::endl << std::endl;
}

void GenerateUniqueSerializer(std::ostream& out, const google::protobuf::FieldDescriptor* field, std::string name = "")
{
    if (name.empty())
        name = cpp::FieldName(field);

    out << "{" << std::endl;
    {
        Indent _(out);

        if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
        {
            out << name << ".serialize(aWriter);" << std::endl;
        }
        else if (field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
        {
            out << "Serialization::WriteString(aWriter, " << name << ");" << std::endl;
        }
        else if (field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
        {
            out << "Serialization::WriteBytes(aWriter, " << name << ");" << std::endl;
        }
        else
        {
            if (field->type() == google::protobuf::FieldDescriptor::TYPE_FIXED32 || field->type() == google::protobuf::FieldDescriptor::TYPE_SFIXED32 ||
                field->type() == google::protobuf::FieldDescriptor::TYPE_FLOAT)
            {
                out << "aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&" << name << "), 32);" << std::endl;
            }
            else if (
                field->type() == google::protobuf::FieldDescriptor::TYPE_FIXED64 || field->type() == google::protobuf::FieldDescriptor::TYPE_SFIXED64 ||
                field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
            {
                out << "aWriter.WriteBits(*reinterpret_cast<const uint64_t*>(&" << name << "), 64);" << std::endl;
            }
            else if (field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL)
            {
                out << "Serialization::WriteBool(aWriter, " << name << ");" << std::endl;
            }
            else
            {
                out << "Serialization::WriteVarInt(aWriter, " << name << ");" << std::endl;
            }
        }
    }
    out << "}" << std::endl << std::endl;
}

void GenerateSerializer(std::ostream& out, const google::protobuf::FieldDescriptor* field)
{
    out << "{" << std::endl;
    {
        Indent _(out);

        if (field->is_map())
        {
            auto itor_name = "internal_" + cpp::FieldName(field);

            out << "Serialization::WriteVarInt(aWriter, " << cpp::FieldName(field) << ".size() & 0xFFFF);" << std::endl;
            out << "for (const auto& [ " << itor_name << "_key, " << itor_name << "_value] : " << cpp::FieldName(field) << ")" << std::endl;
            out << "{" << std::endl;
            {
                Indent _(out);
                GenerateUniqueSerializer(out, field->message_type()->map_key(), itor_name + "_key");
                GenerateUniqueSerializer(out, field->message_type()->map_value(), itor_name + "_value");
            }
            out << "}" << std::endl;
        }
        else if (field->is_repeated())
        {
            out << "Serialization::WriteVarInt(aWriter, " << cpp::FieldName(field) << ".size() & 0xFFFF);" << std::endl;
            out << "for (const auto& element : " << cpp::FieldName(field) << ")" << std::endl;
            GenerateUniqueSerializer(out, field, "element");
        }
        else
        {
            GenerateUniqueSerializer(out, field);
        }
    }
    out << "}" << std::endl << std::endl;
}

void GenerateSource(std::filesystem::path aPath, const google::protobuf::FileDescriptor* apFile, Context& ctx)
{
    std::ostringstream out;

    auto name = cpp::StripProto(apFile->name());
    std::filesystem::path p(name);
    out << "#include \"ProtocolPCH.h\"" << std::endl;
    out << fmt::format(R"(#include "{}.gen.h")", p.filename().string()) << std::endl;
    out << std::endl;
    out << "namespace " << apFile->package() << std::endl;
    out << "{" << std::endl;

    const Context::Message* protocol = nullptr;

    for (const auto& msg : ctx.messages)
    {
        if (cpp::ClassName(msg.descriptor) == "Protocol")
        {
            protocol = &msg;
            continue;
        }

        // Serialize
        out << "bool " << cpp::ClassName(msg.descriptor) << "::serialize(Buffer::Writer& aWriter) const" << std::endl;
        out << "{" << std::endl;
        {
            Indent _{out};
            if (!msg.optional_fields.empty())
            {
                if (msg.optional_fields.size() <= 64)
                {
                    out << "aWriter.WriteBits(internal_data_bitfield_.to_ullong(), " << msg.optional_fields.size() << ");" << std::endl;
                }
                else
                {
                    out << "for(auto i = 0u; i < " << msg.optional_fields.size() << "; ++i)" << std::endl;
                    out << "{" << std::endl;
                    {
                        Indent _{out};
                        out << "Serialization::WriteBool(aReader), internal_data_bitfield_[i]);" << std::endl;
                    }
                    out << "}" << std::endl;
                }
            }

            Set<const google::protobuf::FieldDescriptor*> processed;

            out << "// Optional fields" << std::endl << std::endl;
            for (auto i = 0u; i < msg.optional_fields.size(); ++i)
            {
                auto field = msg.optional_fields[i];
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                out << "if (has_" << cpp::FieldName(field)  << "())" << std::endl;

                GenerateSerializer(out, field);
            }

            out << "// Required fields" << std::endl << std::endl;

            for (auto field : msg.optimized_order)
            {
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                GenerateSerializer(out, field);
            }

            out << "return true;" << std::endl;
        }
        out << "}" << std::endl << std::endl;

        // Deserialize
        out << "bool " << cpp::ClassName(msg.descriptor) << "::deserialize(Buffer::Reader& aReader)" << std::endl;
        out << "{" << std::endl;
        {
            Indent _{out};
            out << "uint64_t scratch = 0;" << std::endl << std::endl;

            Set<const google::protobuf::FieldDescriptor*> processed;

            out << "// Optional fields" << std::endl << std::endl;

            if (!msg.optional_fields.empty())
            {
                if (msg.optional_fields.size() <= 64)
                {
                    out << "aReader.ReadBits(scratch, " << msg.optional_fields.size() << ");" << std::endl;
                    out << "internal_data_bitfield_ |= scratch;" << std::endl;
                }
                else
                {
                    out << "for(auto i = 0u; i < " << msg.optional_fields.size() << "; ++i)" << std::endl;
                    out << "{" << std::endl;
                    {
                        Indent _{out};
                        out << "if(Serialization::ReadBool(aReader)) internal_data_bitfield_.set(i);" << std::endl;
                    }
                    out << "}" << std::endl;
                }
            }

            for (auto i = 0u; i < msg.optional_fields.size(); ++i)
            {
                auto field = msg.optional_fields[i];
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                out << "if (has_" << cpp::FieldName(field) << "())" << std::endl;

                GenerateDeserializer(out, field);
            }

            out << "// Required fields" << std::endl << std::endl;

            for (auto field : msg.optimized_order)
            {
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                GenerateDeserializer(out, field);
            }
            out << "return true;" << std::endl;
        }
        out << "}" << std::endl << std::endl;

        // Has field
        for (auto i = 0u; i < msg.optional_fields.size(); ++i)
        {
            auto field = msg.optional_fields[i];

            out << "bool " << cpp::ClassName(msg.descriptor) << "::has_" << cpp::FieldName(field) << "() const noexcept" << std::endl;
            out << "{" << std::endl;
            {
                Indent _{out};
                out << "return internal_data_bitfield_[" << i << "];" << std::endl;
            }
            out << "}" << std::endl << std::endl;
        }

        // Set field
        {
            Set<const google::protobuf::FieldDescriptor*> processed;
            for (auto i = 0u; i < msg.optional_fields.size(); ++i)
            {
                auto field = msg.optional_fields[i];
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                out << "void " << cpp::ClassName(msg.descriptor) << "::set_" << cpp::FieldName(field) << "(" << GetType(field) << " val) noexcept" << std::endl;
                out << "{" << std::endl;
                {
                    Indent _{out};
                    if (field->containing_oneof())
                        out << "internal_data_bitfield_.reset();" << std::endl;
                    out << "internal_data_bitfield_.set(" << i << ");" << std::endl;
                    out << cpp::FieldName(field) << " = std::move(val);" << std::endl;
                }
                out << "}" << std::endl << std::endl;
            }

            out << "// Required fields" << std::endl << std::endl;

            for (auto field : msg.optimized_order)
            {
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                out << "void " << cpp::ClassName(msg.descriptor) << "::set_" << cpp::FieldName(field) << "(" << GetType(field) << " val) noexcept" << std::endl;
                out << "{" << std::endl;
                {
                    Indent _{out};
                    out << cpp::FieldName(field) << " = std::move(val);" << std::endl;
                }
                out << "}" << std::endl << std::endl;
            }
        }

        // Get field
        {
            Set<const google::protobuf::FieldDescriptor*> processed;
            for (auto i = 0u; i < msg.optional_fields.size(); ++i)
            {
                auto field = msg.optional_fields[i];
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                out << "const " << GetType(field) << "& " << cpp::ClassName(msg.descriptor) << "::get_" << cpp::FieldName(field) << "() const noexcept" << std::endl;
                out << "{" << std::endl;
                {
                    Indent _{out};
                    out << "return " << cpp::FieldName(field) << ";" << std::endl;
                }
                out << "}" << std::endl << std::endl;
            }

            out << "// Required fields" << std::endl << std::endl;

            for (auto field : msg.optimized_order)
            {
                if (processed.contains(field))
                    continue;

                processed.insert(field);

                out << "const " << GetType(field) << "& " << cpp::ClassName(msg.descriptor) << "::get_" << cpp::FieldName(field) << "() const noexcept" << std::endl;
                out << "{" << std::endl;
                {
                    Indent _{out};
                    out << "return " << cpp::FieldName(field) << ";" << std::endl;
                }
                out << "}" << std::endl << std::endl;
            }
        }
    }

    if (protocol)
    {
        GenerateProtocolSource(*protocol, out);
    }

    out << "} // namespace " << apFile->package() << std::endl;

    SaveFile(aPath, out.str().c_str());
}

int main(int argc, char** argv)
{
    cxxopts::Options options("netpack", "Generation of C++ code from a .proto file");
    options.add_options()("path", "File to generate code for.", cxxopts::value<std::string>());
    options.add_options()("output", "Directory to dump into.", cxxopts::value<std::string>());
    options.parse_positional({"path", "output"});
    const auto result = options.parse(argc, argv);
    const auto file = std::filesystem::path(result["path"].as<std::string>());
    const auto output = std::filesystem::path(result["output"].as<std::string>());

    if (file.string().find("internal.proto") != std::string::npos)
        return 0;

    try
    {
        DiskSourceTree tree;
        tree.MapPath("", std::filesystem::current_path().string().c_str());
        tree.MapPath("", absolute(file).parent_path().string().c_str());
        ErrorHandler collector;
        Importer importer(&tree, &collector);

        std::string path = file.relative_path().string();
        std::ranges::replace(path, '\\', '/');

        auto pDescriptor = importer.Import(path);

        const auto headerName = file.filename().replace_extension(".gen.h");
        const auto sourceName = file.filename().replace_extension(".gen.cpp");

        const auto headerPath = output / headerName;
        const auto sourcePath = output / sourceName;

        Context ctx;

        std::vector<const google::protobuf::Descriptor*> descriptors;
        cpp::FlattenMessagesInFile(pDescriptor, &descriptors);

        Set<const google::protobuf::Descriptor*> doneDescriptors;
        for (auto desc : descriptors)
        {
            if (cpp::IsMapEntryMessage(desc))
                continue;

            if (doneDescriptors.contains(desc))
                continue;

            doneDescriptors.insert(desc);

            std::vector<const google::protobuf::FieldDescriptor*> next_order;
            Context::Message msg;
            msg.descriptor = desc;

            for (auto field : cpp::OneOfRange(desc))
            {
                msg.oneof_fields.push_back(field);
            }

            for (auto field : FieldRange(desc))
            {
                if (field->name() == "unreliable")
                {
                    msg.reliable = false;
                    continue;
                }
                if (field->is_optional())
                    msg.optional_fields.push_back(field);

                if (!field->real_containing_oneof())
                    msg.optimized_order.push_back(field);
                else
                    next_order.push_back(field);
            }
            msg.optimized_order.insert(std::end(msg.optimized_order), std::begin(next_order), std::end(next_order));
            ctx.messages.push_back(msg);
        }

        std::filesystem::create_directories(output);

        GenerateHeader(headerPath, pDescriptor, ctx);
        GenerateSource(sourcePath, pDescriptor, ctx);
    }
    catch (std::exception e)
    {
        std::cerr << e.what();
        return -1;
    }

    return 0;
}
