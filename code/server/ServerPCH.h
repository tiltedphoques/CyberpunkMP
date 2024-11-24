#pragma once

#include <iostream>
#include <filesystem>
#include <bitset>
#include <fstream>

#include <flecs.h>

#include <Core/Platform.h>
#include <Core/Stl.h>
#include <Core/Initializer.h>
#include <Core/Buffer.h>
#include <Core/ViewBuffer.h>
#include <Core/Serialization.h>
#include <Core/ScratchAllocator.h>
#include <Math/Math.h>
#include <Core/TaskQueue.h>
#include <Network/Packet.h>

#include <glm/glm.hpp>

#include <gsl/gsl>

#include <ProtocolPCH.h>

#include <coreclr_delegates.h>

#undef check

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <Network/Server.h>
#include <steam/isteamnetworkingutils.h>

#include <nlohmann/json.hpp>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

#include <client.gen.h>
#include <server.gen.h>