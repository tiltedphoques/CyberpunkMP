#pragma once

#include "Math/Vector2.h"


struct GridCell
{
    using TPosition = Vector2<int16_t>;

    GridCell(TPosition aPosition);
    ~GridCell();

    void Add(flecs::entity aEntity) noexcept;
    void Remove(flecs::entity aEntity) noexcept;

    [[nodiscard]] TPosition GetPosition() const noexcept { return m_position; }
    [[nodiscard]] size_t Count() const noexcept { return m_entities.size(); }

    template <class T> void ForEach(T func)
    {
        for (auto pChar : m_entities)
            func(pChar);
    }

    template <class T, class... Args> void ForEach(T func)
    {
        for (auto pChar : m_entities)
        {
            if(pChar.has<Args...>())
                func(pChar);
        }
    }

private:

    Vector<flecs::entity> m_entities;
    TPosition m_position;
};
