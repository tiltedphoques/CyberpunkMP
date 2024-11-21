#pragma once

namespace Game
{
Red::WorldPosition ToRed(const glm::vec3& acPosition);
glm::vec3 ToGlm(const Red::WorldPosition& acPosition);
Red::Quaternion ToRed(const glm::quat& acOrientation);
glm::quat ToGlm(const Red::Quaternion& acOrientation);
}

struct CMPStream
{
public:
    virtual void * GetMemoryPool(void) const 
    { // 00
        return RED4ext::Memory::PoolSerializable::Get();
    }

    virtual ~CMPStream(void) {} // 08
};

struct IMapper
{
public:
    IMapper(void) {}

    virtual ~IMapper(void) {} // 00
    virtual void MapName(Red::CName, unsigned short &) {}; // 08
    virtual void MapType(Red::CBaseRTTIType const *, unsigned short &) {}; // 10
    virtual void MapPointer(Red::Handle<Red::ISerializable> const &, int &) {}; // 18
    virtual void MapResourceReference(void) {}; // 20
    virtual void MapResourceDeferredReference(void) {}; // 28
    virtual void MapBuffer(void *, unsigned short &) {}; // 30

private:
    virtual void UnmapName(unsigned short, Red::CName &) {}; // 38
    virtual void UnmapType(unsigned short, Red::CBaseRTTIType const * &) {}; // 40
    virtual void UnmapPointer(int, Red::Handle<Red::ISerializable> &) {}; // 48
    virtual void UnmapResourceReference(unsigned short, uint64_t *, void *) {}; // 50
    virtual void UnmapResourceDeferredReference(unsigned short, uint64_t *) {}; // 58
    virtual void UnmapBuffer(void) {}; // 60
    virtual void sub_68(void) {}; // 68
    virtual void sub_70(void) {}; // 70
};


struct CMPWriter : public CMPStream
{
public:
    CMPWriter(void) {}

    virtual ~CMPWriter(void) override {} // 08

    virtual void Serialize(uint8_t * aBytes, unsigned __int64 aSize) 
    { // 10
        for (uint64_t i = 0; i < aSize; i++) {
            bytes.push_back(aBytes[i]);
        }
    }

    virtual unsigned __int64 GetOffset(void) const 
    { // 18
        return bytes.size();
    }

    virtual unsigned __int64 GetSize(void) const 
    { // 20
        return bytes.size();
    }

    virtual void Seek(__int64) {} // 28

    virtual void Flush(void) {} // 30

    virtual void ClearError(void) {} // 38

    virtual void GetFileNameForDebug(void) {} // 40

    virtual uint8_t GetSaveVersion(void) const 
    { // 48
        return version;
    }

    virtual uint32_t GetGameVersion(void) const 
    { // 50
        return 1;
    }

    virtual Red::CString GetName(void) const 
    { // 58
        return Red::CString();
    }

    virtual Red::CString GetName2(void) const 
    { // 60
        return Red::CString();
    }

    virtual void Begin(void) {} // 68

    virtual bool EnterNode(Red::CName, bool) 
    { // 70
        return true;
    }

    virtual void LeaveNode(void) {} // 78

    virtual bool NodeFound(void) const 
    { // 80
        return true;
    }

    virtual bool sub_88(void) const 
    { // 88
        return true;
    }

    virtual bool IsOK(void) const 
    { // 90
        return true;
    }

    virtual bool sub_98(void) const 
    { // 98
        return true;
    }

    uint8_t mode = 1;
    uint8_t type = 0;
    uint8_t status = 0;
    uint8_t unk0B; // 0B
    uint8_t version = 0xC3;
    void * mapper = NULL;
    Vector<uint8_t> bytes;
};

struct CMPReader : public CMPStream
{
public:
    CMPReader(Vector<uint8_t> aBytes) 
    {
        bytes = aBytes;
    }

    virtual ~CMPReader(void) override {} // 08

    virtual void Serialize(uint8_t * aBytes, unsigned __int64 aSize) 
    { // 10
        for (uint64_t i = 0; i < aSize; i++) {
            if (offset < bytes.size()) {
                aBytes[i] = bytes[offset++];
            }
        }
    }

    virtual unsigned __int64 GetOffset(void) const 
    { // 18
        return offset;
    }

    virtual unsigned __int64 GetSize(void) const 
    { // 20
        return bytes.size();
    }

    virtual void Seek(__int64) {} // 28

    virtual void Flush(void) {} // 30

    virtual void ClearError(void) {} // 38

    virtual void GetFileNameForDebug(void) {} // 40

    virtual uint8_t GetSaveVersion(void) const 
    { // 48
        return version;
    }

    virtual uint32_t GetGameVersion(void) const 
    { // 50
        return 1;
    }

    virtual Red::CString GetName(void) const 
    { // 58
        return Red::CString();
    }

    virtual Red::CString GetName2(void) const 
    { // 60
        return Red::CString();
    }

    virtual void Begin(void) {} // 68

    virtual bool EnterNode(Red::CName, bool) 
    { // 70
        return true;
    }

    virtual void LeaveNode(void) {} // 78

    virtual bool NodeFound(void) const 
    { // 80
        return true;
    }

    virtual bool sub_88(void) const 
    { // 88
        return true;
    }

    virtual bool IsOK(void) const 
    { // 90
        return true;
    }

    virtual bool sub_98(void) const 
    { // 98
        return true;
    }

    uint8_t mode = 2; // 08
    uint8_t type = 0; // 09
    uint8_t status = 0; // 0A
    uint8_t unk0B; // 0B
    uint8_t version = 0xC3;
    IMapper * mapper = NULL; // 10
    Vector<uint8_t> bytes;
    uint64_t offset = 0;

};