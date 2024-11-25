#pragma once

#include "IAttachmentComponent.h"

struct AttachmentComponentScriptInstance final : IAttachmentComponent
{
    AttachmentComponentScriptInstance(flecs::entity aPuppet);
    ~AttachmentComponentScriptInstance() override;

    // Inherited via IAttachmentComponent
    virtual uint64_t GetParent() override;
    virtual uint64_t GetSlot() override;

protected:
    flecs::entity m_puppet;
};
