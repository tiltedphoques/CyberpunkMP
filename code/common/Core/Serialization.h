#pragma once

#include "Buffer.h"
#include "Stl.h"


struct Serialization
{
    static String ReadString(Buffer::Reader& aReader);
    static void WriteString(Buffer::Writer& aWriter, const String& acString);

    static Vector<uint8_t> ReadBytes(Buffer::Reader& aReader);
    static void WriteBytes(Buffer::Writer& aWriter, const Vector<uint8_t>& acBytes);

    static uint64_t ReadVarInt(Buffer::Reader& aReader);
    static void WriteVarInt(Buffer::Writer& aWriter, uint64_t aValue);

    static uint8_t ReadUint8(Buffer::Reader& aReader);
    static void WriteUint8(Buffer::Writer& aWriter, uint8_t aValue);

    static int8_t ReadInt8(Buffer::Reader& aReader);
    static void WriteInt8(Buffer::Writer& aWriter, int8_t aValue);

    static uint16_t ReadUint16(Buffer::Reader& aReader);
    static void WriteUint16(Buffer::Writer& aWriter, uint16_t aValue);

    static int16_t ReadInt16(Buffer::Reader& aReader);
    static void WriteInt16(Buffer::Writer& aWriter, int16_t aValue);

    static int32_t ReadInt32(Buffer::Reader& aReader);
    static void WriteInt32(Buffer::Writer& aWriter, int32_t aValue);

    static int64_t ReadInt64(Buffer::Reader& aReader);
    static void WriteInt64(Buffer::Writer& aWriter, int64_t aValue);

    static uint32_t ReadUint32(Buffer::Reader& aReader);
    static void WriteUint32(Buffer::Writer& aWriter, uint32_t aValue);

    static uint64_t ReadUint64(Buffer::Reader& aReader);
    static void WriteUint64(Buffer::Writer& aWriter, uint64_t aValue);

    static bool ReadBool(Buffer::Reader& aReader);
    static void WriteBool(Buffer::Writer& aWriter, bool aValue);

    static double ReadDouble(Buffer::Reader& aReader);
    static void WriteDouble(Buffer::Writer& aWriter, double aValue);

    static float ReadFloat(Buffer::Reader& aReader);
    static void WriteFloat(Buffer::Writer& aWriter, float aValue);
};

