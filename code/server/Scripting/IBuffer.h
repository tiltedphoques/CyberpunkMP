#pragma once

#include "ScriptingBase.h"

struct IBuffer
{
    TP_EXPORT static IBuffer* Create();
    TP_EXPORT static void Release(IBuffer* Buffer);

    virtual ~IBuffer(){}

    virtual const char* ReadString() = 0;
    virtual int8_t ReadInt8() = 0;
    virtual uint8_t ReadUint8() = 0;
    virtual int16_t ReadInt16() = 0;
    virtual uint16_t ReadUint16() = 0;
    virtual int32_t ReadInt32() = 0;
    virtual uint32_t ReadUint32() = 0;
    virtual int64_t ReadInt64() = 0;
    virtual uint64_t ReadUint64() = 0;
    virtual uint64_t ReadVarInt() = 0;
    virtual float ReadFloat() = 0;
    virtual double ReadDouble() = 0;
    virtual bool ReadBool() = 0;

    virtual void WriteString(const char* Str) = 0;
    virtual void WriteInt8(int8_t Data) = 0;
    virtual void WriteUint8(uint8_t Data) = 0;
    virtual void WriteInt16(int16_t Data) = 0;
    virtual void WriteUint16(uint16_t Data) = 0;
    virtual void WriteInt32(int32_t Data) = 0;
    virtual void WriteUint32(uint32_t Data) = 0;
    virtual void WriteInt64(int64_t Data) = 0;
    virtual void WriteUint64(uint64_t Data) = 0;
    virtual void WriteVarInt(uint64_t Data) = 0;
    virtual void WriteFloat(float Data) = 0;
    virtual void WriteDouble(int32_t Data) = 0;
    virtual void WriteBool(bool Data) = 0;
};