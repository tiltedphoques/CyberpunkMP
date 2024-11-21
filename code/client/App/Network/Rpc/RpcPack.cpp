#include "RpcPack.h"

#define DECLARE_SIMPLE(x, t)                                     \
    struct x##ArgumentPack : BaseArgumentPack                \
    {                                                            \
        x##ArgumentPack(Red::CBaseRTTIType* type)            \
            : BaseArgumentPack(type)                     \
        {                                                        \
        }                                                        \
                                                                 \
        void Read(void* pDest, Buffer::Reader& aReader) override \
        {                                                        \
            *(t*)pDest = Serialization::Read##x(aReader);        \
        }                                                        \
        void Write(void* pSrc, Buffer::Writer& aWriter) override \
        {                                                        \
             Serialization::Write##x(aWriter, *(t*)pSrc);        \
        }                                                        \
    };

DECLARE_SIMPLE(Bool, bool);
DECLARE_SIMPLE(Float, float);
DECLARE_SIMPLE(Double, double);
DECLARE_SIMPLE(Int8, int8_t);
DECLARE_SIMPLE(Int16, int16_t);
DECLARE_SIMPLE(Int32, int32_t);
DECLARE_SIMPLE(Int64, int64_t);
DECLARE_SIMPLE(Uint8, uint8_t);
DECLARE_SIMPLE(Uint16, uint16_t);
DECLARE_SIMPLE(Uint32, uint32_t);
DECLARE_SIMPLE(Uint64, uint64_t);

struct TWeakDBIDPack : BaseArgumentPack
{
    TWeakDBIDPack(Red::CBaseRTTIType* type)
        : BaseArgumentPack(type)
    {
    }

    void Read(void* pDest, Buffer::Reader& aReader) override
    {
        static_cast<Red::TweakDBID*>(pDest)->name.hash = Serialization::ReadUint32(aReader);
        static_cast<Red::TweakDBID*>(pDest)->name.length = Serialization::ReadUint8(aReader);
    }

    void Write(void* pSrc, Buffer::Writer& aWriter) override
    {
        Serialization::WriteUint32(aWriter, static_cast<Red::TweakDBID*>(pSrc)->name.hash);
        Serialization::WriteUint8(aWriter, static_cast<Red::TweakDBID*>(pSrc)->name.length);
    }
};

struct CNamePack : BaseArgumentPack
{
    CNamePack(Red::CBaseRTTIType* type)
        : BaseArgumentPack(type)
    {
    }

    void Read(void* pDest, Buffer::Reader& aReader) override
    {
        static_cast<Red::CName*>(pDest)->hash = Serialization::ReadUint64(aReader);
    }

    void Write(void* pSrc, Buffer::Writer& aWriter) override
    {
        Serialization::WriteUint64(aWriter, static_cast<Red::CName*>(pSrc)->hash);
    }
};

struct StringPack : BaseArgumentPack
{
    StringPack(Red::CBaseRTTIType* type)
        : BaseArgumentPack(type)
    {
    }

    void Read(void* pDest, Buffer::Reader& aReader) override
    {
        const auto str = Serialization::ReadString(aReader);
        *static_cast<Red::CString*>(pDest) = Red::CString(str.c_str());
    }

    void Write(void* pSrc, Buffer::Writer& aWriter) override
    {
        const auto gameStr = static_cast<Red::CString*>(pSrc);
        const String str(gameStr->c_str(), gameStr->Length());
        Serialization::WriteString(aWriter, str);
    }
};

struct EnumArgumentPack : BaseArgumentPack
{
    EnumArgumentPack(Red::CBaseRTTIType* type)
        : BaseArgumentPack(type)
    {
    }

    void Read(void* pDest, Buffer::Reader& aReader) override
    {
        const auto val = Serialization::ReadVarInt(aReader);
        switch (m_pRtti->GetSize())
        {
        case sizeof(uint8_t): *static_cast<uint8_t*>(pDest) = static_cast<uint8_t>(val); break;
        case sizeof(uint16_t): *static_cast<uint16_t*>(pDest) = static_cast<uint16_t>(val); break;
        case sizeof(uint32_t): *static_cast<uint32_t*>(pDest) = static_cast<uint32_t>(val); break;
        case sizeof(uint64_t): *static_cast<uint64_t*>(pDest) = static_cast<uint64_t>(val); break;
        }
    }

    void Write(void* pSrc, Buffer::Writer& aWriter) override
    {
        switch (m_pRtti->GetSize())
        {
        case sizeof(uint8_t): Serialization::WriteVarInt(aWriter, *static_cast<uint8_t*>(pSrc)); break;
        case sizeof(uint16_t): Serialization::WriteVarInt(aWriter, *static_cast<uint16_t*>(pSrc)); break;
        case sizeof(uint32_t): Serialization::WriteVarInt(aWriter, *static_cast<uint32_t*>(pSrc)); break;
        case sizeof(uint64_t): Serialization::WriteVarInt(aWriter, *static_cast<uint64_t*>(pSrc)); break;
        }
    }
};

struct ClassArgumentPack : BaseArgumentPack
{
    ClassArgumentPack(Red::CBaseRTTIType* type)
        : BaseArgumentPack(type)
    {
        for (const auto t = static_cast<Red::CClass*>(type); const auto prop : t->props)
        {
            m_readers.emplace_back(MakePack(prop->type));
        }
    }

    void Read(void* pDest, Buffer::Reader& aReader) override
    {
        const auto type = static_cast<Red::CClass*>(m_pRtti);
        const auto count = type->props.size;
        for (auto i = 0; i < count; ++i)
        {
            const auto prop = type->props[i];
            const auto dest = prop->GetValuePtr<void>(pDest);
            m_readers[i]->Read(dest, aReader);
        }
    }

    void Write(void* pSrc, Buffer::Writer& aWriter) override
    {
        const auto type = static_cast<Red::CClass*>(m_pRtti);
        const auto count = type->props.size;
        for (auto i = 0; i < count; ++i)
        {
            const auto prop = type->props[i];
            const auto src = prop->GetValuePtr<void>(pSrc);
            m_readers[i]->Write(src, aWriter);
        }
    }

private:
    Vector<UniquePtr<BaseArgumentPack>> m_readers;
};

struct ArrayArgumentPack : BaseArgumentPack
{
    ArrayArgumentPack(Red::CBaseRTTIType* type)
        : BaseArgumentPack(type)
    {
        const auto arr = static_cast<Red::CRTTIArrayType*>(m_pRtti);
        m_reader = MakePack(arr->innerType);
    }

    void Read(void* pDest, Buffer::Reader& aReader) override
    {
        const uint16_t length = Serialization::ReadUint16(aReader);
        const auto type = static_cast<Red::CRTTIArrayType*>(m_pRtti);

        type->Resize(pDest, length);
        for (auto i = 0; i < length; ++i)
        {
            const auto pElement = type->GetElement(pDest, i);
            m_reader->Read(pElement, aReader);
        }
    }

    void Write(void* pSrc, Buffer::Writer& aWriter) override
    {
        const auto type = static_cast<Red::CRTTIArrayType*>(m_pRtti);
        const uint16_t length = type->GetLength(pSrc) & 0xFFFF;
        Serialization::WriteUint16(aWriter, length);

        for (auto i = 0; i < length; ++i)
        {
            const auto src = type->GetElement(pSrc, i);
            m_reader->Write(src, aWriter);
        }
    }

private:

    UniquePtr<BaseArgumentPack> m_reader;
};


UniquePtr<BaseArgumentPack> MakePack(Red::CBaseRTTIType* type)
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
    static auto s_tdbidType = Red::ResolveType<Red::TweakDBID>();
    static auto s_doubleType = Red::ResolveType<double>();
    static auto s_stringType = Red::ResolveType<Red::CString>();

    if (type->GetType() == Red::ERTTIType::Class)
        return MakeUnique<ClassArgumentPack>(type);
    if (type->GetType() == Red::ERTTIType::Array)
        return MakeUnique<ArrayArgumentPack>(type);
    if (type->GetType() == Red::ERTTIType::Enum)
        return MakeUnique<EnumArgumentPack>(type);
    if (type == s_floatType)
        return MakeUnique<FloatArgumentPack>(type);
    if (type == s_boolType)
        return MakeUnique<BoolArgumentPack>(type);
    if (type == s_int8Type)
        return MakeUnique<Int8ArgumentPack>(type);
    if (type == s_uint8Type)
        return MakeUnique<Uint8ArgumentPack>(type);
    if (type == s_int16Type)
        return MakeUnique<Int16ArgumentPack>(type);
    if (type == s_uint16Type)
        return MakeUnique<Uint16ArgumentPack>(type);
    if (type == s_int32Type)
        return MakeUnique<Int32ArgumentPack>(type);
    if (type == s_uint32Type)
        return MakeUnique<Uint32ArgumentPack>(type);
    if (type == s_int64Type)
        return MakeUnique<Int64ArgumentPack>(type);
    if (type == s_uint64Type)
        return MakeUnique<Uint64ArgumentPack>(type);
    if (type == s_nameType)
        return MakeUnique<CNamePack>(type);
    if (type == s_tdbidType)
        return MakeUnique<TWeakDBIDPack>(type);
    if (type == s_doubleType)
        return MakeUnique<DoubleArgumentPack>(type);
    if (type == s_stringType)
        return MakeUnique<StringPack>(type);

    return {};
}
