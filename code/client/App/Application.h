#pragma once

#include "Core/Foundation/Application.hpp"
#include <RED4ext/Api/Sdk.hpp>


namespace App
{
struct Application : Core::Application
{
    explicit Application(RED4ext::PluginHandle aPlugin, const RED4ext::Sdk* aSdk = nullptr);

protected:

    void OnStarted() override;
    void OnStopped() override;
};

extern UniquePtr<Application> GApplication;
}


