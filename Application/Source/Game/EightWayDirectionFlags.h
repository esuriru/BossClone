#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

enum class EightWayDirectionFlags : unsigned char
{
    Up = 1 << 0,
    Up_Right = 1 << 1,
    Right = 1 << 2,
    Down_Right = 1 << 3,
    Down = 1 << 4,
    Down_Left = 1 << 5,
    Left = 1 << 6,
    Up_Left = 1 << 7,
};

enum class EightWayDirection : unsigned char
{
    None,
    Up,
    Up_Right,
    Right,
    Down_Right,
    Down,
    Down_Left,
    Left,
    Up_Left,
};

DEFINE_ENUM_FLAG_OPERATORS(EightWayDirectionFlags);
