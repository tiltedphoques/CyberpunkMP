#include "Packet.h"
#include "SteamInterface.h"


Packet::Packet() noexcept
    : m_pData(nullptr)
    , m_size(0)
{
}

Packet::Packet(const uint32_t aSize) noexcept
    : m_pData(nullptr)
    , m_size(aSize + 1)
{
    m_pData = static_cast<char*>(GetAllocator()->Allocate(m_size));
    m_pData[0] = kPayload;
}

Packet::~Packet() noexcept
{
    GetAllocator()->Free(m_pData);
}

char* Packet::GetData() const noexcept
{
    return m_pData + 1;
}

uint32_t Packet::GetSize() const noexcept
{
    return m_size - 1;
}

uint32_t Packet::GetTotalSize() const noexcept
{
    return m_size;
}

bool Packet::IsValid() const noexcept
{
    return m_pData != nullptr;
}

PacketView::PacketView(char* aPointer, uint32_t aSize)
    : Packet()
{
    m_pData = aPointer;
    m_size = aSize;

    m_pData[0] = kPayload;
}

PacketView::~PacketView()
{
    m_pData = nullptr;
    m_size = 0;
}

