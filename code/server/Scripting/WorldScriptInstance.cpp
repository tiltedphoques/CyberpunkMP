#include "WorldScriptInstance.h"

#include "GameServer.h"

TP_EXPORT IWorld* IWorld::Get()
{
    return GServer->GetWorld()->GetScriptInstance();
}

WorldScriptInstance::WorldScriptInstance()
{

}

WorldScriptInstance::~WorldScriptInstance()
{
}

void WorldScriptInstance::Initialize()
{
    m_callback = GServer->GetScripting()->GetFunction(L"Update");
    if (!m_callback)
        spdlog::error("Unable to find Update function in Sdk, server will crash!!!");

    m_updateSystem = GServer->GetWorld()->system("Script Update")
    .kind(flecs::OnUpdate)
    .iter([this](flecs::iter& iter)
    {
        float delta = iter.delta_time();
        if (m_callback != nullptr)
        {
            m_callback(&delta, sizeof(delta));
        }
        
    });

    m_updateSystem.child_of(GServer->GetWorld()->entity("systems"));
}

