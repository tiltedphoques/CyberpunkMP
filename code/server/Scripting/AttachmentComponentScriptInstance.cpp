#include "Components/AttachmentComponent.h"
#include "AttachmentComponentScriptInstance.h"
#include <Components/PlayerComponent.h>


AttachmentComponentScriptInstance::AttachmentComponentScriptInstance(flecs::entity aPuppet)
    : m_puppet(aPuppet)
{

}

AttachmentComponentScriptInstance::~AttachmentComponentScriptInstance()
{
}

uint64_t AttachmentComponentScriptInstance::GetParent()
{
    const auto pComponent = m_puppet.get<PlayerComponent>();
    if (pComponent)
    {
        auto pAttachmentComponent = pComponent->Puppet.get<AttachmentComponent>();
        if (pAttachmentComponent)
        {
            return pAttachmentComponent->Parent.raw_id();
        }
    }
    return 0;
}

uint64_t AttachmentComponentScriptInstance::GetSlot()
{
    const auto pComponent = m_puppet.get<PlayerComponent>();
    if (pComponent)
    {
        auto pAttachmentComponent = pComponent->Puppet.get<AttachmentComponent>();
        if (pAttachmentComponent)
        {
            return pAttachmentComponent->SlotId;
        }
    }
    return 0;
}
