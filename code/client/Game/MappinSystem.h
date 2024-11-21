#include "RED4ext/Scripting/Natives/Generated/game/mappins/IMappinSystem.hpp"
#include "RED4ext/Scripting/Natives/Generated/game/mappins/MappinData.hpp"
#include "RED4ext/Scripting/Natives/Generated/game/NewMappinID.hpp"
#include "RED4ext/Scripting/Natives/Generated/game/mappins/MappinScriptData.hpp"
#include "RED4ext/NativeTypes.hpp"
#include "RED4ext/Scripting/Natives/Generated/Vector3.hpp"

constexpr auto RegisterRemotePlayerMappin = Core::RawFunc<2624990479UL, 
    Red::game::NewMappinID * (*)(Red::game::mappins::IMappinSystem *, Red::game::NewMappinID *, Red::game::mappins::MappinData const &, Red::WeakHandle<Red::game::Object> *)>();


constexpr auto RegisterMappin = Core::RawFunc<3577160229UL, 
    Red::game::NewMappinID * (*)(Red::game::mappins::IMappinSystem *, Red::game::NewMappinID *, Red::game::mappins::MappinData const &, Red::Vector3 *)>();
