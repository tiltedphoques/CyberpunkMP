#include "IBuffer.h"

struct BufferScriptInstance : IBuffer
{
    BufferScriptInstance();
    ~BufferScriptInstance() override;
    const char* ReadString() override;
    int32_t ReadInt32() override;
    uint32_t ReadUint32() override;
    int64_t ReadInt64() override;
    uint64_t ReadUint64() override;
    uint64_t ReadVarInt() override;
    float ReadFloat() override;
    double ReadDouble() override;
    void WriteString(const char* Str) override;
    void WriteInt32(int32_t Data) override;
    void WriteUint32(uint32_t Data) override;
    void WriteInt64(int64_t Data) override;
    void WriteUint64(uint64_t Data) override;
    void WriteVarInt(uint64_t Data) override;
    void WriteFloat(float Data) override;
    void WriteDouble(int32_t Data) override;
    bool ReadBool() override;
    void WriteBool(bool Data) override;
    int8_t ReadInt8() override;
    uint8_t ReadUint8() override;
    int16_t ReadInt16() override;
    uint16_t ReadUint16() override;
    void WriteInt8(int8_t Data) override;
    void WriteUint8(uint8_t Data) override;
    void WriteInt16(int16_t Data) override;
    void WriteUint16(uint16_t Data) override;

    const uint8_t* GetData() const { return m_buffer.GetData(); }
    size_t Size() const { return m_writer.Size(); }

    void Set(const Vector<uint8_t>& aData);

private:

    Buffer m_buffer;
    Buffer::Writer m_writer;
    Buffer::Reader m_reader;
};
