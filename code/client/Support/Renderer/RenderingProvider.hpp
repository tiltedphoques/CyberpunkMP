#pragma once

#include "Core/Foundation/Feature.hpp"
#include "Core/Hooking/HookingAgent.hpp"
#include "Core/Logging/LoggingAgent.hpp"
#include "Core/Rendering/RenderingDriver.hpp"

namespace Support
{
class RenderingProvider
    : public Core::Feature
    , public Core::RenderingDriver
    , public Core::HookingAgent
    , public Core::LoggingAgent
{
protected:
    void OnInitialize() override;

    void Present(int32_t* apDeviceIndex, uint8_t aSomeSync, UINT aSyncInterval);
    void Resize(uint32_t aWidth, uint32_t aHeight, uint32_t a3, uint8_t a4, int* apDeviceIndex) const;
    void Shutdown(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) const;

    static void PresentHook(int32_t* apDeviceIndex, uint8_t aSomeSync, UINT aSyncInterval);
    static void ResizeHook(uint32_t aWidth, uint32_t aHeight, uint32_t a3, uint8_t a4, int* apDeviceIndex);
    static void ShutdownHook(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4);

    static LRESULT APIENTRY WndProc(HWND ahWnd, UINT auMsg, WPARAM awParam, LPARAM alParam);

    bool m_initialized = false;

private:

    struct FrameContext
    {
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;
        Microsoft::WRL::ComPtr<ID3D12Resource> BackBuffer;
        D3D12_CPU_DESCRIPTOR_HANDLE MainRenderTargetDescriptor{0};
    };

    WNDPROC m_wndProc{nullptr};
    Vector<FrameContext> m_frameContexts;

    void Initialize();
};
}
