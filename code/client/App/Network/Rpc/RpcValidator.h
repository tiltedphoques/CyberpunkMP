#pragma once
#include "Core/Hooking/HookingAgent.hpp"

struct RpcValidator : Core::HookingAgent
{
    static void InternalValidate();

    static void Attach();
};
