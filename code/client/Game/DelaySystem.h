#include "RED4ext/Scripting/Natives/Generated/game/IDelaySystem.hpp"

namespace Game
{
struct ExDelaySystem : Red::game::IDelaySystem
{
    enum DelayFlag : uint32_t
    {
        NoFlag = 0,
        Flag1 = 1 << 0,
        Flag2 = 1 << 1,
        Flag3 = 1 << 2,
        Flag4 = 1 << 3
    };

    virtual void sub_1B0();
    virtual void sub_1B8();
    virtual void sub_1C0();
    virtual void AddFunction(const std::function<void()>& func, float delay, DelayFlag flag, int one, Red::CName name1, Red::CName name2);
    
};
}