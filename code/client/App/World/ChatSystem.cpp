#include "ChatSystem.h"

#include "App/Network/NetworkService.h"
#include "App/ChatMessageEvent.h"
#include <RED4ext/Scripting/Natives/Generated/game/ui/IGameSystemUI.hpp>

void ChatSystem::OnWorldAttached(RED4ext::world::RuntimeScene* aScene)
{
    m_ready = true;
}

void ChatSystem::OnAfterWorldDetach()
{
    m_ready = false;
}

void ChatSystem::HandleChatMessage(const PacketEvent<server::ChatMessage>& aMessage)
{
    m_messages.push_back(aMessage);

    auto evt = reinterpret_cast<ChatMessageUIEvent*>(Red::GetClass("ChatMessageUIEvent")->CreateInstance());
    evt->author = RED4ext::CString(aMessage.get_username());
    evt->message = RED4ext::CString(aMessage.get_message());

    auto uiSystem = Red::GetGameSystem<RED4ext::game::ui::IGameSystemUI>();
    uiSystem->QueueEvent(RED4ext::Handle(evt));
}

void ChatSystem::OnInitialize(const RED4ext::JobHandle& aJob)
{
    const auto pNetworkService = Core::Container::Get<NetworkService>();
    pNetworkService->RegisterHandler<&ChatSystem::HandleChatMessage>(this);
}

void ChatSystem::Send(const Red::CString& aMessage)
{
    const auto pNetworkService = Core::Container::Get<NetworkService>();

    client::ChatMessageRequest request;
    request.set_message(aMessage.c_str());

    pNetworkService->Send(request);
}

RED4ext::CString ChatSystem::GetUsername()
{
    return RED4ext::CString("jackhumbert");
}
