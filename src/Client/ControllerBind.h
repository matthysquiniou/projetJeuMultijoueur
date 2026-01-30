#pragma once
#include "cpu-engine.h"

enum class ControllerBind : int
{
    PLAYER_FORWARD = 'Z',
    PLAYER_BACK = 'S',
    PLAYER_RIGHT = 'D',
    PLAYER_LEFT = 'Q',
    PLAYER_SHOOT = VK_LBUTTON,
    PLAYER_FOLLOW_LAST_SHOOT = VK_RBUTTON,
};