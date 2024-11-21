#include "Core/Stl.hpp"
#include <RED4ext/Scripting/Natives/Generated/ink/TextInputWidget.hpp>

struct TextInputExtension : RED4ext::ink::TextInputWidget
{
    void MoveCaret(int32_t position) { this->caretPosition = position; }
};

RTTI_EXPAND_CLASS(RED4ext::ink::TextInputWidget, { RTTI_METHOD_FQN(TextInputExtension::MoveCaret); });