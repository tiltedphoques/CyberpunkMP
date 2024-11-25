#include "ServerAPI.h"

#include "GameServer.h"

#include "WorldScriptInstance.h"
#include <PlayerManager.h>

static UniquePtr<GameServer> s_server;

void ServerAPI::Initialize()
{
    s_server = MakeUnique<GameServer>();
}

void ServerAPI::Run()
{
    s_server->Run();
}

void ServerAPI::Exit()
{
    s_server = nullptr;
}

TP_EXPORT void ServerAPI::SetUpdateCallback(TUpdateCallback apCallback)
{
    GServer->GetWorld()->GetScriptInstance()->SetUpdateCallback(apCallback);
}

void ServerAPI::SetPlayerJoinCallback(TPlayerEvent callback)
{
    GServer->GetWorld()->get_mut<PlayerManager>()->GetScriptInstance()->SetPlayerJoinCallback(callback);
}

void ServerAPI::SetPlayerLeftCallback(TPlayerEvent callback)
{
    GServer->GetWorld()->get_mut<PlayerManager>()->GetScriptInstance()->SetPlayerLeftCallback(callback);
}
