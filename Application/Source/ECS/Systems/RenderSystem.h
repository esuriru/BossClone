#pragma once
#include "ECS/System.h"
#include "Core/Timestep.h"
#include "ECS/Entity.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Renderer/SubTexture2D.h"

class SpriteRenderSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

private:
};

class TilemapRenderSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

};

class RunningAnimationSystem : public System
{
public:
    auto Update(Timestep ts) -> void;
    auto OnEvent(Event& e) -> void;

private:
    auto OnAnimationEvent(AnimationEvent& e) -> bool;

    std::array<size_t, MaxEntities> spriteIterators_{};
    std::array<size_t, MaxEntities> frameCounters_{};
    std::array<Ref<SubTexture2D>, MaxEntities> originalTextures_{};

};
