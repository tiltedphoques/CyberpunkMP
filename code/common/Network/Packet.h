#pragma once

#include "Core/Meta.h"
#include "Core/Allocator.h"


struct Packet
{
    TP_ALLOCATOR;

    Packet() noexcept;
    Packet(uint32_t aSize) noexcept;
    virtual ~Packet() noexcept;

    TP_NOCOPYMOVE(Packet);

    // Get the start of the write buffer
    [[nodiscard]] char* GetData() const noexcept;

    // Get the writable data size, note that the actual packet size may differ from that
    [[nodiscard]] uint32_t GetSize() const noexcept;

    // Get the writable data size + protocol data size
    [[nodiscard]] uint32_t GetTotalSize() const noexcept;

    // Returns true if the packet has an associated buffer, usually used to check if the underlying allocator had enough space
    [[nodiscard]] bool IsValid() const noexcept;

protected:

    friend struct Server;
    friend struct Client;

    char* m_pData;
    uint32_t m_size;
};

struct PacketView : Packet
{
    PacketView(char* aPointer, uint32_t aSize);
    virtual ~PacketView();
};

