#pragma once

#include "Core/Foundation/Feature.hpp"
#include <Core/TaskQueue.h>

struct ThreadService final
    : Core::Feature
{
    static void RunInMainThread(std::function<void()> aFunc);

protected:

    void OnGameUpdate(RED4ext::CGameApplication* apApp) override;

private:

    TaskQueue m_queue;
};
