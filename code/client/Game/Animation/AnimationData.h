#pragma once

#include "Base.h"

struct AnimationData
{
    Red::CName controller;
    bool debug = false;
    Locomotion_Style style{LS_Idle};
    MotionTableAction action{MTA_None};
    float path = 0.f;
    float slope = 0.f;
    float delta = 0.f;
    bool stairs = false;
    float ground = 0.f;
    float time = 0.f;
    float footScale = 1.f;
    float startAngle = 0.f;
    bool unk = false;
    float speed = 0.f;
};
