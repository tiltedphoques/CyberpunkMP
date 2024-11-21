#pragma once

#include <cstdint>
#include "Allocator.h"


struct Buffer : AllocatorCompatible
{
    Buffer() noexcept;
    explicit Buffer(size_t aSize) noexcept;
    explicit Buffer(const uint8_t* apData, size_t aSize) noexcept;
    Buffer(const Buffer& acBuffer) noexcept;
    Buffer(Buffer&& aBuffer) noexcept;
    virtual ~Buffer();

    uint8_t operator[](size_t aIndex) const noexcept;
    uint8_t& operator[](size_t aIndex) noexcept;

    Buffer& operator=(const Buffer& acBuffer) noexcept;
    Buffer& operator=(Buffer&& aBuffer) noexcept;

    [[nodiscard]] size_t GetSize() const noexcept;
    bool Resize(size_t aSize) noexcept;

    [[nodiscard]] const uint8_t* GetData() const noexcept;
    [[nodiscard]] uint8_t* GetWriteData() const noexcept;

    struct Cursor
    {
        explicit Cursor(Buffer* apBuffer);

        void Reset();
        bool Eof() const;
        void Advance(size_t aByteCount);
        void Reverse(size_t aByteCount);

        [[nodiscard]] size_t Size() const noexcept;
        [[nodiscard]] size_t GetBytePosition() const;
        [[nodiscard]] size_t GetBitPosition() const;

        [[nodiscard]] Buffer* GetBuffer() const noexcept;
        [[nodiscard]] uint8_t* GetDataAtPosition() const noexcept;

    protected:

        size_t m_bitPosition;
        Buffer* m_pBuffer;
    };

    struct Reader : Cursor
    {
        explicit Reader(Buffer* apBuffer);

        bool ReadBits(uint64_t& aDestination, size_t aCount);
        bool ReadBytes(uint8_t* apDestination, size_t aCount);
    };

    struct Writer : Cursor
    {
        explicit Writer(Buffer* apBuffer);

        bool WriteBits(uint64_t aData, size_t aCount);
        bool WriteBytes(const uint8_t* apSource, size_t aCount);
    };

protected:

    uint8_t* m_pData;
    size_t m_size;
};

