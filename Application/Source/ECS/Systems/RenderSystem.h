#pragma once

#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/System.h"

#include "Core/Timestep.h"
#include "Events/Event.h"
#include "Events/EventDispatcher.h"
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

using AnimType = Animation::AnimationType;
template<typename T> struct AnimationTypeMap;
template<> struct AnimationTypeMap<RunningAnimationComponent>
{
    static constexpr AnimType value = AnimType::Running;
};

template<> struct AnimationTypeMap<SwingingAnimationComponent>
{
    static constexpr AnimType value = AnimType::Swinging;
};

template<typename T>
AnimType GetEnumValue() { return AnimationTypeMap<T>::value; }

template<typename T>
class AnimationSystem : public System
{
public:
    inline auto Update(Timestep ts) -> void
    {
        for (auto& e : entities)
        {
            auto& animationComponent = coordinator->GetComponent<T>(e);
            if (!animationComponent.Enabled)
            {
                return;
            }

            auto& spriteRenderer = coordinator->GetComponent<SpriteRendererComponent>(e);

            auto& animation = swingingAnimation.Animation;
            if (animation.AnimationIndices.empty())
            {
                return;
            }

            spriteRenderer.Texture = animation.SpriteTextures[animation.AnimationIndices[spriteIterators_[e]]];

            if (++frameCounters_[e] >= animation.FramesBetweenTransition)
            {
                // If the sprite shown is already the last sprite, go back to the first loop.
                if (animation.AnimationIndices.size() - 1 == spriteIterators_[e])
                {
                    spriteIterators_[e] = 0;    
                }
                else
                {
                    ++spriteIterators_[e];
                }
                frameCounters_[e] -= animation.FramesBetweenTransition;
            }
        }

    } 
    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher;
        dispatcher.Dispatch<AnimationEvent>(CC_BIND_EVENT_FUNC(AnimationSystem::OnAnimationEvent));
    }

    inline auto SetOriginalTexture(Ref<SubTexture2D> origTexture, Entity e) -> void
    {
        originalTextures_[e] = origTexture;
    }

private:
    auto OnAnimationEvent(AnimationEvent& e) -> bool
    {
        if (e.GetAnimationType() != GetEnumValue<T>())
            return false;

        Entity entity = e.GetEntityAffected(); 

        auto& runningAnimation = coordinator->GetComponent<RunningAnimationComponent>(entity); 
        runningAnimation.Enabled = e.IsAnimationEnabled();

        auto& spriteRenderer = coordinator->GetComponent<SpriteRendererComponent>(entity); 

        if (!e.IsAnimationEnabled())
        {
            // Reset everything back to what it was.
            spriteIterators_[entity] = 0;
            frameCounters_[entity] = 0;

            // NOTE - If the original texture was nothing, then this will set a different texture.
            if (originalTextures_ == nullptr)
                spriteRenderer.Texture = originalTextures_[entity];
        }
        else
        {
            // Cache the texture so we can restore it back when the animation is cancelled/stopped.
            originalTextures_[entity] = spriteRenderer.Texture;
        }

        return true;
    }

    std::array<size_t, MaxEntities> spriteIterators_{};
    std::array<size_t, MaxEntities> frameCounters_{};
    std::array<Ref<SubTexture2D>, MaxEntities> originalTextures_{};
    static Coordinator* coordinator = Coordinator::Instance();
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

class SwingingAnimationSystem : public System
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

