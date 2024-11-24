#include "GridCell.h"
#include "Components/CellComponent.h"

GridCell::GridCell(TPosition aPosition)
    : m_position(aPosition)
{
    
}

GridCell::~GridCell()
{
}

void GridCell::Add(flecs::entity aEntity) noexcept
{
    m_entities.push_back(aEntity);
    aEntity.emplace<CellComponent>(this);
}


void GridCell::Remove(flecs::entity aEntity) noexcept
{
    aEntity.remove<CellComponent>();
    m_entities.erase(std::ranges::find(m_entities, aEntity));
}

/*void GridCell::Load(Player* apPlayer, NotifyCharacterLoad& aMessage) const noexcept
{
    NotifyCharacterLoad message;
    message.Data.push_back(apPlayer->Serialize());

    for (const auto pChar : m_players)
    {
        pChar->Send(message);
        aMessage.Data.push_back(pChar->Serialize());
    }

    for (const auto pChar : m_npcs)
    {
        aMessage.Data.push_back(pChar->Serialize());
    }
}

void GridCell::Unload(Player* apPlayer, NotifyCharacterUnload& aMessage) const noexcept
{
    NotifyCharacterUnload message;
    message.Id.push_back(apPlayer->GetId());

    for (const auto pChar : m_players)
    {
        pChar->Send(message);

        aMessage.Id.push_back(pChar->GetId());
    }

    for (const auto pChar : m_npcs)
    {
        aMessage.Id.push_back(pChar->GetId());
    }
}*/
