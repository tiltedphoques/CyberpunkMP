#pragma once

#include "RenderingDriver.hpp"

namespace Core
{
class RenderingAgent
{
protected:
    static RenderingDriver& GetRenderingDriver();

    RenderingAgent();
    virtual ~RenderingAgent();

    TP_NOCOPYMOVE(RenderingAgent);

    virtual void OnRenderInit() = 0;
    virtual void OnPresent() = 0;
    virtual void OnReset(bool aCompleteReset) = 0;
    virtual bool OnWindowProc(HWND ahWnd, UINT auMsg, WPARAM awParam, LPARAM alParam) { return false; }

    static Microsoft::WRL::ComPtr<IDXGISwapChain4> GetSwapChain();
    static Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue();
    static HWND GetWindow();

    friend RenderingDriver;

private:
    friend RenderingDriver;

    static void SetDriver(RenderingDriver& aDriver);
};
}
