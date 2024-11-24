#pragma once

#include "GridCell.h"

struct World;

struct Level
{
    static GridCell::TPosition ToCell(const glm::vec3& acLocation) noexcept;

    Level(World* apWorld) noexcept;
    Level(Level&& aLevel) noexcept;
    Level& operator=(Level&& aLevel) noexcept;

    void Add(flecs::entity aEntity) noexcept;
    void Remove(flecs::entity aEntity) noexcept;

    gsl::not_null<GridCell*> GetCell(GridCell::TPosition aPosition) noexcept;
    gsl::not_null<World*> GetWorld() noexcept { return m_pWorld; }

    template <class T> void ForEachInRange(const GridCell* pCell, int16_t aRange, T func) noexcept
    {
        const auto pos = pCell->GetPosition();
        for (int i = pos.x - aRange; i < (aRange + pos.x); ++i)
            for (int j = pos.y - aRange; j < (aRange + pos.y); ++j)
            {
                const auto itor = m_cells.find(GridCell::TPosition{static_cast<int16_t>(i), static_cast<int16_t>(j)});
                if (itor != std::end(m_cells))
                {
                    itor.value()->ForEach(func);
                }
            }
    }

    template <class T> void ForEachCellInRange(const GridCell* pCell, int16_t aRange, T func) noexcept
    {
        const auto pos = pCell->GetPosition();
        for (int i = pos.x - aRange; i < (aRange + pos.x); ++i)
            for (int j = pos.y - aRange; j < (aRange + pos.y); ++j)
            {
                const auto itor = m_cells.find(GridCell::TPosition{static_cast<int16_t>(i), static_cast<int16_t>(j)});
                if (itor != std::end(m_cells))
                {
                    func(itor.value().get());
                }
            }
    }

    void Test() noexcept;

protected:
    friend struct Handle;

    void Update(flecs::entity aEntity) noexcept;
    void AddPlayer(flecs::entity aEntity) noexcept;
    void RemovePlayer(flecs::entity aEntity) noexcept;

    void HandleSpawnCharacterRequest(PacketEvent<client::SpawnCharacterRequest>& aMessage) noexcept;
    void HandleMoveEntityRequest(PacketEvent<client::MoveEntityRequest>& aMessage) noexcept;
    void HandleEnterVehicleRequest(PacketEvent<client::EnterVehicleRequest>& aMessage) noexcept;
    void HandleExitVehicleRequest(PacketEvent<client::ExitVehicleRequest>& aMessage) noexcept;

    static server::NotifyCharacterLoad Serialize(flecs::entity aEntity) noexcept;

    void TransferCell(flecs::entity aEntity, GridCell* apOldCell, GridCell* apNewCell) noexcept;
    void CollectCells(const GridCell* apNewCell, const GridCell* apOldCell, Set<GridCell*>& aToLoad, Set<GridCell*>& aToUnload) noexcept;

private:
    Map<GridCell::TPosition, UniquePtr<GridCell>> m_cells;
    World* m_pWorld;
    flecs::system m_updateSystem;
};
