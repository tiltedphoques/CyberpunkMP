#include "RED4ext/Scripting/Natives/Generated/game/Puppet.hpp"
#include "RED4ext/Scripting/Natives/Generated/move/Component.hpp"

namespace Game
{

struct ExPuppet : Red::game::Puppet
{
    static constexpr const char* NAME = "gamePuppet";
    static constexpr const char* ALIAS = NAME;

    struct TeleportArg
    {
        Red::WeakHandle<Red::game::Object> Object;
        Red::Vector4 Position;
        Red::EulerAngles Rotation;
    };

    virtual void sub_108();
    virtual void sub_110();
    virtual void sub_118();
    virtual void sub_120();
    virtual void sub_128();
    virtual void sub_130();
    virtual void sub_138();
    virtual void sub_140();
    virtual void sub_148();
    virtual void sub_150();
    virtual void sub_158();
    virtual void sub_160();
    virtual void sub_168();
    virtual void sub_170();
    virtual void sub_178();
    virtual void sub_180();
    virtual void sub_188();
    virtual void sub_190();
    virtual void sub_198();
    virtual void sub_1a0();
    virtual void sub_1a8();
    virtual void sub_1b0();
    virtual void sub_1b8();
    virtual void sub_1c0();
    virtual void sub_1c8();
    virtual void sub_1d0();
    virtual void sub_1d8();
    virtual void sub_1e0();
    virtual void sub_1e8();
    virtual void sub_1f0();
    virtual void sub_1f8();
    virtual void sub_200();
    virtual void sub_208();
    virtual void sub_210();
    virtual void Teleport(const TeleportArg&);

    uint8_t pad268[0x420 - 0x268];
    Red::Handle<Red::move::Component> m_moveComponent;
};
}
