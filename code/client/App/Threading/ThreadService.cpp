#include "ThreadService.h"

void ThreadService::RunInMainThread(std::function<void()> aFunc)
{
    auto pService = Core::Container::Get<ThreadService>();
    pService->m_queue.Add(aFunc);
}

void ThreadService::OnGameUpdate(RED4ext::CGameApplication* apApp)
{
    m_queue.Drain();
}
