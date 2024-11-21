#pragma once

struct ClientRpc : RED4ext::IScriptable
{
    RTTI_IMPL_TYPEINFO(ClientRpc);
    RTTI_IMPL_ALLOCATOR();
};

struct ServerRpc : RED4ext::IScriptable
{
    RTTI_IMPL_TYPEINFO(ServerRpc);
    RTTI_IMPL_ALLOCATOR();
};

RTTI_DEFINE_CLASS(ClientRpc, { RTTI_ALIAS("CyberpunkMP.ClientRpc"); });
RTTI_DEFINE_CLASS(ServerRpc, { RTTI_ALIAS("CyberpunkMP.ServerRpc"); });