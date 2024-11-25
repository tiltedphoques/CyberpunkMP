#pragma once

struct World;
struct ChatSystem
{
    ChatSystem(gsl::not_null<World*> apWorld);

    void Broadcast(String acUsername, String acMessage);

protected:

    void HandleChatMessageRequest(const PacketEvent<client::ChatMessageRequest>& aMessage);

private:

    gsl::not_null<World*> m_pWorld;
};

