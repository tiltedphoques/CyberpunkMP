#pragma once

#include "Stl.h"
#include <mutex>


// Thread safe task queue
struct TaskQueue
{
    void Add(std::function<void()> aFunction) noexcept;
    bool Pop(std::function<void()>& aResultFunction) noexcept;
    size_t Drain() noexcept;

    void UnsafeAdd(std::function<void()> aFunction) noexcept;
    bool UnsafePop(std::function<void()>& aResultFunction) noexcept;

    [[nodiscard]] std::mutex& Lock() noexcept;

private:

    std::mutex m_mutex;
    Queue<std::function<void()>> m_tasks;
};

