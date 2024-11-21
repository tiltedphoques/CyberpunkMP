#include "Application.hpp"

Core::Application::Application(RED4ext::PluginHandle aPlugin, const RED4ext::Sdk* aSdk)
{
    RED4ext::GameState state{nullptr, nullptr, nullptr};
    state.OnUpdate = &RunUpdate;

    aSdk->gameStates->Add(aPlugin, RED4ext::EGameStateType::Running, &state);

    s_pApp = this;
}

Core::Application::~Application()
{
    s_pApp = nullptr;
}

void Core::Application::Bootstrap()
{
    if (m_booted)
        return;

    if (!s_discoveryCallbacks.empty())
    {
        for (const auto& callback : s_discoveryCallbacks)
        {
            callback(*this);
        }
        s_discoveryCallbacks.clear();
    }

    m_booted = true;

    OnStarting();

    for (const auto& feature : GetRegistered())
    {
        feature->OnBootstrap();
    }

    OnStarted();
}

void Core::Application::Shutdown()
{
    if (!m_booted)
        return;

    OnStopping();

    for (const auto& feature : GetRegistered())
    {
        feature->OnShutdown();
    }

    OnStopped();

    m_booted = false;
}

void Core::Application::Update(RED4ext::CGameApplication* apApp) const
{
    for (const auto& feature : GetRegistered())
    {
        feature->OnGameUpdate(apApp);
    }
}

void Core::Application::OnRegistered(const SharedPtr<Feature>& aFeature)
{
    aFeature->OnRegister();

    if (m_booted)
    {
        aFeature->OnBootstrap();
    }
}

bool Core::Application::Discover(AutoDiscoveryCallback aCallback)
{
    s_discoveryCallbacks.push_back(aCallback);
    return true;
}

bool Core::Application::RunUpdate(RED4ext::CGameApplication* apApp)
{
    if (s_pApp)
        s_pApp->Update(apApp);
    return false;
}
