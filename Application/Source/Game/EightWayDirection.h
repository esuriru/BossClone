#pragma once

enum class EightWayDirection
{
    None = 0,
    Up = 1 << 0,
    Up_Right = 1 << 1,
    Right = 1 << 2,
    Down_Right = 1 << 3,
    Down = 1 << 4,
    Down_Left = 1 << 5,
    Left = 1 << 6,
    Up_Left = 1 << 7,
};