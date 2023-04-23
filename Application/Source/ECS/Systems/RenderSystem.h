#pragma once
#include "ECS/System.h"
#include "Core/Timestep.h"

class SpriteRenderSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

private:
};

class TileRenderSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

};
