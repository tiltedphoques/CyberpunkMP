#pragma once

#include "Core/Foundation/Feature.hpp"
#include "Core/Hooking/HookingAgent.hpp"
#include "Core/Logging/LoggingAgent.hpp"
#include "Core/Rendering/RenderingAgent.hpp"

namespace App
{
struct DebugService;

struct ImGuiService : Core::Feature, Core::HookingAgent, Core::LoggingAgent, Core::RenderingAgent
{
    ImGuiService();
    ~ImGuiService() override;

    void OnRenderInit() override;
    void OnPresent() override;
    void OnReset(bool aCompleteReset = false) override;
    bool OnWindowProc(HWND ahWnd, UINT auMsg, WPARAM awParam, LPARAM alParam) override;
    void OnBootstrap() override;

protected:

    void OnGameUpdate(RED4ext::CGameApplication* apApp) override;

    void PrepareUpdate();

private:

    struct FrameContext
    {
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;
        Microsoft::WRL::ComPtr<ID3D12Resource> BackBuffer;
        D3D12_CPU_DESCRIPTOR_HANDLE MainRenderTargetDescriptor{0};
    };

    Vector<FrameContext> m_frameContexts;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList{};
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pSrvDescHeap{};
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pRtvDescHeap{};
    std::recursive_mutex m_imguiLock;
    std::array<ImDrawData, 3> m_imguiDrawDataBuffers;
    ImGuiStyle m_styleReference{};
    bool m_initialized = false;
};
}
