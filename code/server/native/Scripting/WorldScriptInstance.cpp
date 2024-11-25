#include "WorldScriptInstance.h"

#include "GameServer.h"

TP_EXPORT IWorld* IWorld::Get()
{
    return GServer->GetWorld()->GetScriptInstance();
}

void WorldScriptInstance::SetUpdateCallback(TUpdateCallback apCallback)
{
    m_callback = apCallback;
}

WorldScriptInstance::WorldScriptInstance()
{

}

WorldScriptInstance::~WorldScriptInstance()
{
}

void WorldScriptInstance::Initialize()
{
    m_updateSystem = GServer->GetWorld()->system("Script Update")
    .kind(flecs::OnUpdate)
    .run([this](flecs::iter& iter)
    {
        float delta = iter.delta_time();
        if (m_callback != nullptr)
            m_callback(delta);
    });

    m_updateSystem.child_of(GServer->GetWorld()->entity("systems"));
}

