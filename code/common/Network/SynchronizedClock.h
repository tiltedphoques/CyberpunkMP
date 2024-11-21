#pragma once

#include <cstdint>


struct SynchronizedClock
{
    SynchronizedClock() noexcept;
    [[nodiscard]] uint64_t GetCurrentTick() const noexcept;
    [[nodiscard]] bool IsSynchronized() const noexcept;
    void Synchronize(uint64_t aServerTick, uint32_t aPing) noexcept;
    void Reset() noexcept;
    void Update() noexcept;

private:

    uint64_t m_lastServerTick;
    uint64_t m_simulatedTick;
    std::chrono::nanoseconds m_previousSimulatedTick;
    std::chrono::nanoseconds m_tickDelta;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastSynchronizationTime{};
};

