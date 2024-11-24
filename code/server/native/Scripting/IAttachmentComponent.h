#pragma once

#include "ScriptingBase.h"

struct TP_EXPORT IAttachmentComponent
{
    CS_INTERNAL IAttachmentComponent() = default;
    IAttachmentComponent(const IAttachmentComponent&) = delete;
    virtual ~IAttachmentComponent() {}

    virtual uint64_t GetParent() = 0;
    virtual uint64_t GetSlot() = 0;
};
