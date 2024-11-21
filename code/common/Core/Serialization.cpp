#include "Serialization.h"


String Serialization::ReadString(Buffer::Reader& aReader)
{
    String result;

    const uint16_t len = ReadVarInt(aReader) & 0xFFFF;
    result.resize(len);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(result.data()), len);

    return result;
}

void Serialization::WriteString(Buffer::Writer& aWriter, const String& acString)
{
    const uint16_t len = acString.size() & 0xFFFF;

    WriteVarInt(aWriter, len);
    aWriter.WriteBytes(reinterpret_cast<const uint8_t*>(acString.data()), len);
}


Vector<uint8_t> Serialization::ReadBytes(Buffer::Reader& aReader) 
{
    Vector<uint8_t> result;

    const uint16_t len = ReadVarInt(aReader) & 0xFFFF;
    result.resize(len);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(result.data()), len);

    return result;
}

void Serialization::WriteBytes(Buffer::Writer& aWriter, const Vector<uint8_t>& acBytes) 
{
    const uint16_t len = acBytes.size() & 0xFFFF;

    WriteVarInt(aWriter, len);
    aWriter.WriteBytes(reinterpret_cast<const uint8_t*>(acBytes.data()), len);
}

uint64_t Serialization::ReadVarInt(Buffer::Reader& aReader)
{
    uint64_t value = 0;
    uint32_t shift = 0;

    do
    {
        uint64_t tmp = 0;
        aReader.ReadBits(tmp, 7);

        value |= (tmp << shift);

        shift += 7;
        
    } while (ReadBool(aReader));

    return value;
}

void Serialization::WriteVarInt(Buffer::Writer& aWriter, uint64_t aValue)
{
    do
    {
        aWriter.WriteBits(aValue, 7);

        aValue >>= 7;

        WriteBool(aWriter, aValue != 0);
    } while (aValue > 0);
}

uint8_t Serialization::ReadUint8(Buffer::Reader& aReader)
{
    uint64_t dest;
    aReader.ReadBits(dest, 8);
    return dest & 0xFF;
}

void Serialization::WriteUint8(Buffer::Writer& aWriter, uint8_t aValue)
{
    aWriter.WriteBits(aValue, 8);
}

int8_t Serialization::ReadInt8(Buffer::Reader& aReader)
{
    uint64_t dest;
    aReader.ReadBits(dest, 8);
    uint8_t tmp = dest & 0xFF;
    return *reinterpret_cast<int8_t*>(&tmp);
}

void Serialization::WriteInt8(Buffer::Writer& aWriter, int8_t aValue)
{
    const uint8_t val = *reinterpret_cast<uint8_t*>(&aValue);
    aWriter.WriteBits(val, 8);
}

uint16_t Serialization::ReadUint16(Buffer::Reader& aReader)
{
    uint64_t dest;
    aReader.ReadBits(dest, 16);
    uint16_t tmp = dest & 0xFFFF;
    return tmp;
}

void Serialization::WriteUint16(Buffer::Writer& aWriter, uint16_t aValue)
{
    const uint16_t val = aValue;
    aWriter.WriteBits(val, 16);
}

int16_t Serialization::ReadInt16(Buffer::Reader& aReader)
{
    uint64_t dest;
    aReader.ReadBits(dest, 16);
    uint16_t tmp = dest & 0xFFFF;
    return *reinterpret_cast<int16_t*>(&tmp);
}

void Serialization::WriteInt16(Buffer::Writer& aWriter, int16_t aValue)
{
    const uint16_t val = *reinterpret_cast<uint16_t*>(&aValue);
    aWriter.WriteBits(val, 16);
}

int32_t Serialization::ReadInt32(Buffer::Reader& aReader)
{
    uint32_t res = ReadVarInt(aReader) & 0xFFFFFFFF;
    auto sign = (res & 1) << 31;
    if (sign)
        res = ~res;
    res >>= 1;
    res |= sign;
    return *(int32_t*)&res;
}

void Serialization::WriteInt32(Buffer::Writer& aWriter, int32_t aValue)
{
    uint32_t value = *(uint32_t*)&aValue;
    uint32_t sign = value >> 31;
    value <<= 1;
    if (sign)
        value = ~value;
    value |= sign;
    WriteVarInt(aWriter, value);
}

int64_t Serialization::ReadInt64(Buffer::Reader& aReader)
{
    uint64_t res = ReadVarInt(aReader);
    auto sign = (res & 1) << 63;
    if (sign)
        res = ~res;
    res >>= 1;
    res |= sign;
    return *(int64_t*)&res;
}

void Serialization::WriteInt64(Buffer::Writer& aWriter, int64_t aValue)
{
    uint64_t value = *(uint64_t*)&aValue;
    uint64_t sign = value >> 63;
    value <<= 1;
    if (sign)
        value = ~value;
    value |= sign;
    WriteVarInt(aWriter, value);
}

uint32_t Serialization::ReadUint32(Buffer::Reader& aReader)
{
    return ReadVarInt(aReader) & 0xFFFFFFFF;
}

void Serialization::WriteUint32(Buffer::Writer& aWriter, uint32_t aValue)
{
    WriteVarInt(aWriter, aValue);
}

uint64_t Serialization::ReadUint64(Buffer::Reader& aReader)
{
    return ReadVarInt(aReader);
}

void Serialization::WriteUint64(Buffer::Writer& aWriter, uint64_t aValue)
{
    WriteVarInt(aWriter, aValue);
}

bool Serialization::ReadBool(Buffer::Reader& aReader)
{
    uint64_t value = 0;
    aReader.ReadBits(value, 1);

    return value == 1;
}

void Serialization::WriteBool(Buffer::Writer& aWriter, bool aValue)
{
    const uint64_t value = aValue ? 1 : 0;
    aWriter.WriteBits(value, 1);
}

double Serialization::ReadDouble(Buffer::Reader& aReader)
{
    double value = 0;
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&value), sizeof(value));

    return value;
}

void Serialization::WriteDouble(Buffer::Writer& aWriter, double aValue)
{
    aWriter.WriteBytes(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
}

float Serialization::ReadFloat(Buffer::Reader& aReader)
{
    float value = 0;
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&value), sizeof(value));

    return value;
}

void Serialization::WriteFloat(Buffer::Writer& aWriter, float aValue)
{
    aWriter.WriteBytes(reinterpret_cast<const uint8_t*>(&aValue), sizeof(aValue));
}

