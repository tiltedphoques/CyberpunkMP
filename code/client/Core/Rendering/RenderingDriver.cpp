#include "RenderingDriver.hpp"
#include "RenderingAgent.hpp"

#include <cassert>

namespace
{
Core::RenderingDriver* s_default;
}

void Core::RenderingDriver::SetDefault(RenderingDriver& aDriver)
{
    s_default = &aDriver;

    RenderingAgent::SetDriver(aDriver);
}

Core::RenderingDriver& Core::RenderingDriver::GetDefault()
{
    assert(s_default);
    return *s_default;
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> Core::RenderingDriver::GetSwapChain() const
{
    return m_pdxgiSwapChain;
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> Core::RenderingDriver::GetCommandQueue() const
{
    return m_pCommandQueue;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Core::RenderingDriver::GetCommandList() const
{
    return m_pCommandList;
}

HWND Core::RenderingDriver::GetWindow() const
{
    return m_window;
}

void Core::RenderingDriver::Register(RenderingAgent* aAgent)
{
    assert(aAgent);
    m_agents.push_back(aAgent);
}

void Core::RenderingDriver::UnRegister(RenderingAgent* aAgent)
{
    const auto itor = std::ranges::find(m_agents, aAgent);
    if (itor != std::end(m_agents))
        m_agents.erase(itor);
}

void Core::RenderingDriver::TriggerPresent() const
{
    for (auto& agent : m_agents)
        agent->OnPresent();
}

void Core::RenderingDriver::TriggerReset(bool aCompleteReset) const
{
    for (auto& agent : m_agents)
        agent->OnReset(aCompleteReset);
}

void Core::RenderingDriver::TriggerRenderInit() const
{
    for (auto& agent : m_agents)
        agent->OnRenderInit();
}

bool Core::RenderingDriver::TriggerWindowProc(HWND ahWnd, UINT auMsg, WPARAM awParam, LPARAM alParam)
{
    for (auto& agent : m_agents)
        if (agent->OnWindowProc(ahWnd, auMsg, awParam, alParam))
            return true;

    return false;
}
