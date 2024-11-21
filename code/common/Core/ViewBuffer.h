#pragma once

#include "Buffer.h"


struct ViewBuffer : Buffer
{
    explicit ViewBuffer(uint8_t* apData, size_t aSize) noexcept;
    virtual ~ViewBuffer();
};

