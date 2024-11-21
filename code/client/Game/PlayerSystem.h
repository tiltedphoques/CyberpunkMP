#include "RED4ext/Scripting/Natives/Generated/cp/PlayerSystem.hpp"
#include "RED4ext/Scripting/Natives/Generated/game/Object.hpp"

namespace Game
{
struct PlayerSystem : Red::PlayerSystem
{
    virtual void sub_1B0(); // 1B0
    virtual void sub_1B8(); // 1B8
    virtual void sub_1C0(); // 1C0
    virtual void sub_1C8(); // 1C8
    virtual void sub_1D0(); // 1D0
    virtual void sub_1D8(); // 1D8
    virtual void GetLocalPlayerControlledGameObject(Red::Handle<Red::GameObject>& aObject); // 1E0
};
}