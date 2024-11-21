#pragma once

#include "ScriptingBase.h"

struct IAttachmentComponent
{
    IAttachmentComponent(const IAttachmentComponent&) = delete;
    virtual ~IAttachmentComponent() {}

    virtual uint64_t GetParent() = 0;
    virtual uint64_t GetSlot() = 0;

protected:

    IAttachmentComponent() = default;
};
