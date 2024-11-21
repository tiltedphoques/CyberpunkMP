#include "BufferScriptInstance.h"


TP_EXPORT IBuffer* IBuffer::Create()
{
    return new BufferScriptInstance();
}

TP_EXPORT void IBuffer::Release(IBuffer* Buffer)
{
    delete static_cast<BufferScriptInstance*>(Buffer);
}

BufferScriptInstance::BufferScriptInstance()
    : m_buffer(1 << 16)
    , m_writer(&m_buffer)
    , m_reader(&m_buffer)
{
}

BufferScriptInstance::~BufferScriptInstance()
{
}

const char* BufferScriptInstance::ReadString()
{
    static String s_buffer;
    s_buffer = Serialization::ReadString(m_reader);
    return s_buffer.c_str();
}

int32_t BufferScriptInstance::ReadInt32()
{
    return Serialization::ReadInt32(m_reader);
}

uint32_t BufferScriptInstance::ReadUint32()
{
    return Serialization::ReadUint32(m_reader);
}

int64_t BufferScriptInstance::ReadInt64()
{
    return Serialization::ReadInt64(m_reader);
}

uint64_t BufferScriptInstance::ReadUint64()
{
    return Serialization::ReadUint64(m_reader);
}

uint64_t BufferScriptInstance::ReadVarInt()
{
    return Serialization::ReadVarInt(m_reader);
}

void BufferScriptInstance::WriteVarInt(uint64_t Data)
{
    Serialization::WriteVarInt(m_writer, Data);
}

float BufferScriptInstance::ReadFloat()
{
    return Serialization::ReadFloat(m_reader);
}

double BufferScriptInstance::ReadDouble()
{
    return Serialization::ReadDouble(m_reader);
}

void BufferScriptInstance::WriteString(const char* Str)
{
    Serialization::WriteString(m_writer, Str);
}

void BufferScriptInstance::WriteInt32(int32_t Data)
{
    Serialization::WriteInt32(m_writer, Data);
}

void BufferScriptInstance::WriteUint32(uint32_t Data)
{
    Serialization::WriteUint32(m_writer, Data);
}

void BufferScriptInstance::WriteInt64(int64_t Data)
{
    Serialization::WriteInt64(m_writer, Data);
}

void BufferScriptInstance::WriteUint64(uint64_t Data)
{
    Serialization::WriteUint64(m_writer, Data);
}

void BufferScriptInstance::WriteFloat(float Data)
{
    Serialization::WriteFloat(m_writer, Data);
}

void BufferScriptInstance::WriteDouble(int32_t Data)
{
    Serialization::WriteDouble(m_writer, Data);
}

bool BufferScriptInstance::ReadBool()
{
    return Serialization::ReadBool(m_reader);
}

void BufferScriptInstance::WriteBool(bool Data)
{
    Serialization::WriteBool(m_writer, Data);
}

int8_t BufferScriptInstance::ReadInt8()
{
    return Serialization::ReadInt8(m_reader);
}

uint8_t BufferScriptInstance::ReadUint8()
{
    return Serialization::ReadUint8(m_reader);
}

int16_t BufferScriptInstance::ReadInt16()
{
    return Serialization::ReadInt16(m_reader);
}

uint16_t BufferScriptInstance::ReadUint16()
{
    return Serialization::ReadUint16(m_reader);
}

void BufferScriptInstance::WriteInt8(int8_t Data)
{
    Serialization::WriteInt8(m_writer, Data);
}

void BufferScriptInstance::WriteUint8(uint8_t Data)
{
    Serialization::WriteUint8(m_writer, Data);
}

void BufferScriptInstance::WriteInt16(int16_t Data)
{
    Serialization::WriteInt16(m_writer, Data);
}

void BufferScriptInstance::WriteUint16(uint16_t Data)
{
    Serialization::WriteUint16(m_writer, Data);
}

void BufferScriptInstance::Set(const Vector<uint8_t>& aData)
{
    std::memcpy(m_buffer.GetWriteData(), aData.data(), aData.size());
}
