#include "ImGuiService.h"

#include "dx12.h"
#include "win32.h"
#include "App/Application.h"
#include "App/Debugging/DebugService.h"

namespace App
{

ImGuiService::ImGuiService()
{
}

ImGuiService::~ImGuiService()
{
}

void ImGuiService::OnRenderInit()
{
    Microsoft::WRL::ComPtr<ID3D12Device> pDevice;
    if (FAILED(GetSwapChain()->GetDevice(IID_PPV_ARGS(&pDevice))))
    {
        return;
    }

    DXGI_SWAP_CHAIN_DESC sdesc;
    GetSwapChain()->GetDesc(&sdesc);

    const auto buffersCounts = std::min(sdesc.BufferCount, 3u);
    m_frameContexts.resize(buffersCounts);

    D3D12_DESCRIPTOR_HEAP_DESC srvdesc = {};
    srvdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvdesc.NumDescriptors = buffersCounts;
    srvdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (FAILED(pDevice->CreateDescriptorHeap(&srvdesc, IID_PPV_ARGS(&m_pSrvDescHeap))))
    {
        LogError("RenderingProvider::Initialize() - failed to create SRV descriptor heap!");
        OnReset();
        return;
    }

    D3D12_DESCRIPTOR_HEAP_DESC rtvdesc;
    rtvdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvdesc.NumDescriptors = buffersCounts;
    rtvdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvdesc.NodeMask = 1;

    if (FAILED(pDevice->CreateDescriptorHeap(&rtvdesc, IID_PPV_ARGS(&m_pRtvDescHeap))))
    {
        LogError("RenderingProvider::Initialize() - failed to create RTV descriptor heap!");
        OnReset();
        return;
    }

    const SIZE_T rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < buffersCounts; i++)
    {
        auto& context = m_frameContexts[i];
        context.MainRenderTargetDescriptor = rtvHandle;
        GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&context.BackBuffer));
        pDevice->CreateRenderTargetView(context.BackBuffer.Get(), nullptr, context.MainRenderTargetDescriptor);
        rtvHandle.ptr += rtvDescriptorSize;
    }

    for (auto& context : m_frameContexts)
        if (FAILED(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&context.CommandAllocator))))
        {
            LogError("RenderingProvider::Initialize() - failed to create command allocator!");
            OnReset();
            return;
        }

    if (FAILED(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frameContexts[0].CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_pCommandList))) ||
        FAILED(m_pCommandList->Close()))
    {
        LogError("RenderingProvider::Initialize() - failed to create command list!");
        OnReset();
        return;
    }

    {
        std::lock_guard _(m_imguiLock);

        // TODO - scale also by DPI
        const auto [resx, resy] = std::tie(sdesc.BufferDesc.Width, sdesc.BufferDesc.Height);
        const auto scaleFromReference = std::min(static_cast<float>(resx) / 1920.0f, static_cast<float>(resy) / 1080.0f);

        auto& io = ImGui::GetIO();
       // io.Fonts->Clear();
        #if 0
        // tmp:
        struct FontSettings
        {
            std::string Path{};
            std::string Language{"Default"};
            float BaseSize{18.0f};
            int32_t OversampleHorizontal{3};
            int32_t OversampleVertical{1};
        } fontSettings;

        ImFontConfig config;
       // const auto& fontSettings = m_options.Font;
        config.SizePixels = std::floorf(fontSettings.BaseSize * scaleFromReference);
        config.OversampleH = fontSettings.OversampleHorizontal;
        config.OversampleV = fontSettings.OversampleVertical;
        if (config.OversampleH == 1 && config.OversampleV == 1)
            config.PixelSnapH = true;
        config.MergeMode = false;
        #endif

        if (ImGui::GetCurrentContext() == nullptr)
        {
            // do this once, do not repeat context creation!
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            // TODO - make this configurable eventually and overridable by mods for themselves easily
            // setup CET default style
            ImGui::StyleColorsDark(&m_styleReference);
            m_styleReference.WindowRounding = 6.0f;
            m_styleReference.WindowTitleAlign.x = 0.5f;
            m_styleReference.ChildRounding = 6.0f;
            m_styleReference.PopupRounding = 6.0f;
            m_styleReference.FrameRounding = 6.0f;
            m_styleReference.ScrollbarRounding = 12.0f;
            m_styleReference.GrabRounding = 12.0f;
            m_styleReference.TabRounding = 6.0f;
        }

        ImGui::GetStyle() = m_styleReference;
        ImGui::GetStyle().ScaleAllSizes(scaleFromReference);

        if (!ImGui_ImplWin32_Init(GetWindow()))
        {
            LogError("RenderingProvider::Initialize() - ImGui_ImplWin32_Init call failed!");
            return;
        }

        if (!ImGui_ImplDX12_Init(
                pDevice.Get(), static_cast<int>(buffersCounts), DXGI_FORMAT_R8G8B8A8_UNORM, m_pSrvDescHeap.Get(), m_pSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                m_pSrvDescHeap->GetGPUDescriptorHandleForHeapStart()))
        {
            LogError("RenderingProvider::Initialize() - ImGui_ImplDX12_Init call failed!");
            ImGui_ImplWin32_Shutdown();
            return;
        }

        // ReloadFonts();

        if (!ImGui_ImplDX12_CreateDeviceObjects())
        {
            LogError("RenderingProvider::Initialize() - ImGui_ImplDX12_CreateDeviceObjects call failed!");
            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            return;
        }
    }

    m_initialized = true;
}

void ImGuiService::OnPresent()
{
    if (!m_initialized)
        return;

    // swap staging ImGui buffer with render ImGui buffer
    {
        std::lock_guard _(m_imguiLock);
        ImGui_ImplDX12_NewFrame();
        if (m_imguiDrawDataBuffers[1].Valid)
        {
            std::swap(m_imguiDrawDataBuffers[0], m_imguiDrawDataBuffers[1]);
            m_imguiDrawDataBuffers[1].Valid = false;
        }
    }

    if (!m_imguiDrawDataBuffers[0].Valid)
        return;

    const auto bufferIndex = GetSwapChain()->GetCurrentBackBufferIndex();
    auto& frameContext = m_frameContexts[bufferIndex];
    frameContext.CommandAllocator->Reset();

    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = frameContext.BackBuffer.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    ID3D12DescriptorHeap* heaps[] = {m_pSrvDescHeap.Get()};

    m_pCommandList->Reset(frameContext.CommandAllocator.Get(), nullptr);
    m_pCommandList->ResourceBarrier(1, &barrier);
    m_pCommandList->SetDescriptorHeaps(1, heaps);
    m_pCommandList->OMSetRenderTargets(1, &frameContext.MainRenderTargetDescriptor, FALSE, nullptr);

    ImGui_ImplDX12_RenderDrawData(&m_imguiDrawDataBuffers[0], m_pCommandList.Get());

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    m_pCommandList->ResourceBarrier(1, &barrier);
    m_pCommandList->Close();

    ID3D12CommandList* commandLists[] = {m_pCommandList.Get()};
    GetCommandQueue()->ExecuteCommandLists(1, commandLists);
}

void ImGuiService::OnReset(bool aCompleteReset)
{
    if (m_initialized)
    {
        std::lock_guard _(m_imguiLock);

        for (auto& drawData : m_imguiDrawDataBuffers)
        {
            for (auto i = 0; i < drawData.CmdListsCount; ++i)
                IM_DELETE(drawData.CmdLists[i]);
            drawData.Clear();
        }

        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();

        if (aCompleteReset)
            ImGui::DestroyContext();
    }

    m_frameContexts.clear();
    
    m_pRtvDescHeap.Reset();
    m_pSrvDescHeap.Reset();
    m_pCommandList.Reset();

    m_initialized = false;
}

bool ImGuiService::OnWindowProc(HWND ahWnd, UINT auMsg, WPARAM awParam, LPARAM alParam)
{
    if (m_initialized)
    {
        if (const auto res = ImGui_ImplWin32_WndProcHandler(ahWnd, auMsg, awParam, alParam))
            return res != 0;
    }

    return false;
}

void ImGuiService::OnBootstrap()
{
    
}

void ImGuiService::OnGameUpdate(RED4ext::CGameApplication* apApp)
{
    PrepareUpdate();
}

void ImGuiService::PrepareUpdate()
{
    if (!m_pCommandList || !m_initialized)
        return;

    std::lock_guard _(m_imguiLock);

    DXGI_SWAP_CHAIN_DESC sdesc;
    GetSwapChain()->GetDesc(&sdesc);

    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // r
    Core::Container::Get<DebugService>()->Draw();
  
    ImGui::Render();

    auto& drawData = m_imguiDrawDataBuffers[2];

    for (auto i = 0; i < drawData.CmdListsCount; ++i)
        IM_DELETE(drawData.CmdLists[i]);
    drawData.Clear();

    drawData = *ImGui::GetDrawData();

    ImVector<ImDrawList*> copiedDrawLists;
    for (auto i = 0; i < drawData.CmdListsCount; ++i)
        copiedDrawLists.push_back(drawData.CmdLists[i]->CloneOutput());
    drawData.CmdLists = copiedDrawLists;

    std::swap(m_imguiDrawDataBuffers[1], m_imguiDrawDataBuffers[2]);
}
}
