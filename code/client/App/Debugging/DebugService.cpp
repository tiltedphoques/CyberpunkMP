
#include "DebugService.h"
#include <App/Network/NetworkService.h>

namespace App
{
    DebugService::DebugService()
    {
    }

    DebugService::~DebugService()
    {
    }

    void DebugService::OnBootstrap()
    {
    }

    void DebugService::Draw()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Test"))
            {
                if (ImGui::Button("Connect"))
                {
                    Core::Container::Get<NetworkService>()->Connect("127.0.0.1:11778");

                    /*auto handle = Red::GetGameSystem<NetworkWorldSystem>();
                    Red::EntityID id;
                    Red::ScriptGameInstance game;
                    Red::CallVirtual(handle, "CreatePuppet", id, game);*/
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

    }
}
