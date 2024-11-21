#pragma once

namespace Core
{
class RenderingAgent;

class RenderingDriver
{
public:

    static void SetDefault(RenderingDriver& aDriver);
    static RenderingDriver& GetDefault();

    Microsoft::WRL::ComPtr<IDXGISwapChain4> GetSwapChain() const;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() const;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const;
    HWND GetWindow() const;

    void Register(RenderingAgent* aAgent);
    void UnRegister(RenderingAgent* aAgent);

protected:

    void TriggerPresent() const;
    void TriggerReset(bool aCompleteReset = false) const;
    void TriggerRenderInit() const;
    bool TriggerWindowProc(HWND, UINT, WPARAM, LPARAM);

    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_pdxgiSwapChain{};
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCommandQueue{};
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList{};
    HWND m_window{nullptr};

private:

    Vector<RenderingAgent*> m_agents;
};
}
