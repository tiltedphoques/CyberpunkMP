#pragma once

#include <RED4ext/Scripting/Natives/Generated/ent/MorphTargetManagerComponent.hpp>
#include <RED4ext/Scripting/Natives/Generated/ent/MorphTargetWeightEntry.hpp>
 
inline Red::DynArray<Red::ent::MorphTargetWeightEntry> * GetTargetWeightEntries(Red::ent::MorphTargetManagerComponent * self) 
{
    return reinterpret_cast<Red::DynArray<Red::ent::MorphTargetWeightEntry> *>((uintptr_t)self + 0x98);
}

constexpr auto SetTargetWeightWithNormalize = Core::RawFunc<633871992UL, 
    void (*)(Red::ent::MorphTargetManagerComponent *, Red::CName targetName, Red::CName regionName, float amount, bool force)>();

constexpr auto ResetWeights = Core::RawFunc<1054086242UL, 
    void (*)(Red::ent::MorphTargetManagerComponent *)>();
    
