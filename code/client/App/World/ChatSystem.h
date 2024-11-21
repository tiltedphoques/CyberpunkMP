#pragma once

#include "Core/Stl.hpp"

struct ChatSystem : RED4ext::IScriptable
{
    RTTI_IMPL_TYPEINFO(ChatSystem);
    RTTI_IMPL_ALLOCATOR();

    void Update(uint64_t aTick);

    void OnInitialize(const RED4ext::JobHandle& aJob);
    void OnWorldAttached(RED4ext::world::RuntimeScene* aScene);
    void OnAfterWorldDetach();

    void Send(const Red::CString& aMessage);
    RED4ext::CString GetUsername();

protected:


    void HandleChatMessage(const PacketEvent<server::ChatMessage>& aMessage);

private:
    bool m_ready{false};
    Vector<server::ChatMessage> m_messages;
};

RTTI_DEFINE_CLASS(ChatSystem, { 
    RTTI_ALIAS("CyberpunkMP.World.ChatSystem");
    RTTI_METHOD(Send);
    RTTI_METHOD(GetUsername);
});