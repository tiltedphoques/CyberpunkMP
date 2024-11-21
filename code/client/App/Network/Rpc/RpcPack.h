#pragma once

struct BaseArgumentPack
{
    BaseArgumentPack(Red::CBaseRTTIType* type)
        : m_pRtti(type)
    {
    }

    virtual ~BaseArgumentPack() {}

    virtual void Read(void* pDest, Buffer::Reader& aReader) = 0;
    virtual void Write(void* pSrc, Buffer::Writer& aWriter) = 0;

    Red::CBaseRTTIType* m_pRtti;
};

UniquePtr<BaseArgumentPack> MakePack(Red::CBaseRTTIType* type);