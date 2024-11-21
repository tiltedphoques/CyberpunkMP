#include "RenderingAgent.hpp"

#include <cassert>

namespace
{
Core::RenderingDriver* s_driver;
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> Core::RenderingAgent::GetSwapChain()
{
    return GetRenderingDriver().GetSwapChain();
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> Core::RenderingAgent::GetCommandQueue()
{
    return GetRenderingDriver().GetCommandQueue();
}

HWND Core::RenderingAgent::GetWindow()
{
    return GetRenderingDriver().GetWindow();
}

void Core::RenderingAgent::SetDriver(RenderingDriver& aDriver)
{
    s_driver = &aDriver;
}

Core::RenderingDriver& Core::RenderingAgent::GetRenderingDriver()
{
    assert(s_driver);
    return *s_driver;
}

Core::RenderingAgent::RenderingAgent()
{
    GetRenderingDriver().Register(this);
}

Core::RenderingAgent::~RenderingAgent()
{
    GetRenderingDriver().UnRegister(this);
}

