#pragma once

#include "Core/Container/Registry.hpp"
#include "Core/Foundation/Feature.hpp"
#include "Core/Stl.hpp"

namespace Core
{
using AutoDiscoveryCallback = void(*)(Application&);

class Application : public Registry<Feature>
{
public:
    template<class T, typename... Args>
    Feature::Defer<T> Register(Args&&... aArgs)
    {
        return Registry<Feature>::Register<T>(std::forward<Args>(aArgs)...);
    }

    Application(RED4ext::PluginHandle aPlugin, const RED4ext::Sdk* aSdk = nullptr);
    virtual ~Application();
    void Bootstrap();
    void Shutdown();

    void Load(RED4ext::CGameApplication* apApp);
    void Update(RED4ext::CGameApplication* apApp) const;

    static bool Discover(AutoDiscoveryCallback aCallback);

protected:

    static bool RunLoad(RED4ext::CGameApplication* apApp);
    static bool RunUpdate(RED4ext::CGameApplication* apApp);

    void OnRegistered(const SharedPtr<Feature>& aFeature) override;

    virtual void OnStarting() {}
    virtual void OnStarted() {}
    virtual void OnStopping() {}
    virtual void OnStopped() {}

private:
    RED4ext::PluginHandle m_plugin;
    const RED4ext::Sdk* m_sdk;
    bool m_booted = false;

    inline static Vector<AutoDiscoveryCallback> s_discoveryCallbacks;
    inline static Application* s_pApp = nullptr;
};
}
