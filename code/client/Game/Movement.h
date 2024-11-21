#include "RED4ext/Scripting/Natives/Generated/game/Puppet.hpp"
#include "RED4ext/SpinLock.hpp"

struct MultiMovementController;

namespace Game
{
struct StubMoveComponent
{
    uint8_t pad[0xA8];
    MultiMovementController* pController;
    uint8_t padB8[0x128 - 0xB0];
    Red::SpinLock lock;
};
} // namespace Game