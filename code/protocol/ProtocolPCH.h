#pragma once

#include <Core/Buffer.h>
#include <Core/Serialization.h>

#include <entt/signal/dispatcher.hpp>

#include <bitset>

template<class T>
struct PacketEvent : T
{
    using Type = T;

    uint32_t ConnectionId{0};
};