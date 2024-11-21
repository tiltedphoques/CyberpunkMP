#pragma once

#define SPDLOG_WCHAR_FILENAMES

#include <iostream>
#include <filesystem>

#include <algorithm>
#include <codecvt>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <future>
#include <map>
#include <memory>
#include <ranges>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <Network/Client.h>
#include <Network/SynchronizedClock.h>

#include <Core/ViewBuffer.h>
#include <Core/Serialization.h>
#include <Core/Buffer.h>

#include <gsl/gsl>

#include <imgui.h>
#include <mem/mem.h>
#include <mem/module.h>
#include <mem/pattern.h>
#include <MinHook.h>

#include <ProtocolPCH.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#undef check

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <nlohmann/json.hpp>

#include <common.gen.h>
#include <client.gen.h>
#include <server.gen.h>

#include <RED4ext/RED4ext.hpp>

#include <RED4ext/Detail/AddressHashes.hpp>
#include <RED4ext/JobQueue.hpp>
#include <RED4ext/ResourceDepot.hpp>
#include <RED4ext/ResourceLoader.hpp>
#include <RED4ext/SystemUpdate.hpp>

#include <RED4ext/Scripting/Natives/Generated/game/IGameSystem.hpp>
#include <RED4ext/Scripting/Natives/Generated/game/ScriptableSystem.hpp>
#include <RED4ext/Scripting/Natives/Generated/world/IRuntimeSystem.hpp>
#include <RED4ext/Scripting/Natives/Generated/game/ITeleportationFacility.hpp>
#include <RED4ext/Scripting/Natives/Generated/ent/EntityID.hpp>
#include <RED4ext/Scripting/Natives/ScriptGameInstance.hpp>

#include <nameof.hpp>
#include <semver.hpp>

#include "Core/Raw.hpp"
#include "Core/Stl.h"

#include "Red/Alias.hpp"
#include "Red/Engine.hpp"
#include "Red/Specializations.hpp"
#include "Red/TypeInfo.hpp"
#include "Red/Utils.hpp"

#include "Game/PlayerSystem.h"
#include "Game/Entity.h"

#include <d3d12.h>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DbgHelp.h>
#include <dxgi1_4.h>
#include <wrl.h>

#include <flecs.h>
