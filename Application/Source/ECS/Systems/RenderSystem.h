#pragma once

#include <glm/glm.hpp>

#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/System.h"
#include "ECS/Coordinator.h"

#include "Core/Timestep.h"
#include "Events/Event.h"
#include "Events/EventDispatcher.h"
#include "Events/ApplicationEvent.h"
#include "Renderer/SubTexture2D.h"

#include "Core/Window.h"

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

using std::array;

using AnimType = Animation::AnimationType;
template<typename T> struct AnimationTypeMap;
template<>
struct AnimationTypeMap<RunningAnimationComponent>
{
    static constexpr AnimType value = AnimType::Running;
    static constexpr const char* debug_name = "RunningAnimationComponent";
};

template<>
struct AnimationTypeMap<SwingingAnimationComponent>
{
    static constexpr AnimType value = AnimType::Swinging;
    static constexpr const char* debug_name = "SwingingAnimationComponent";
};

template<typename T>
constexpr auto GetEnumValue() -> AnimType { return AnimationTypeMap<T>::value; }

template<typename T>
auto GetDebugName() -> std::string { return std::string(AnimationTypeMap<T>::debug_name); }

template<typename T>
class AnimationSystem : public System
{
public:
    EventCallback eventCallback;

    inline auto Update(Timestep ts) -> void
    {
        constexpr float step = 1 / CC_FIXED_UPDATE_FRAME_RATE;
        constexpr float clamp = 1/ 30.f;
        static float accumulator = 0.f;

        accumulator += glm::min(static_cast<float>(ts), clamp);

        while (accumulator >= step) 
        {
            static Coordinator* coordinator = Coordinator::Instance(); 
            for (auto& e : entities)
            {
                auto& animationComponent = coordinator->GetComponent<T>(e);
                if (!animationComponent.Enabled)
                {
                    continue;
                }

                auto& spriteRenderer = coordinator->GetComponent<SpriteRendererComponent>(e);

                auto& animation = animationComponent.Animation;
                if (animation.AnimationIndices.empty())
                {
                    continue;
                }

                // TODO - Maybe not keep updating the texture every frame.
                spriteRenderer.Texture = animation.SpriteTextures[animation.AnimationIndices[spriteIterators_[e]]];
                // CC_TRACE("Sprite iterator for ", GetDebugName<T>(), " : ", spriteIterators_[e]);

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
                    AnimationSpriteChangeEvent event(GetEnumValue<T>(), e, spriteIterators_[e]);
                    eventCallback(event);
                    // spriteRenderer.Texture = animation.SpriteTextures[animation.AnimationIndices[spriteIterators_[e]]];

                    frameCounters_[e] -= animation.FramesBetweenTransition;
                }
            }
            accumulator -= step;
        }
    } 
    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<AnimationEvent>(CC_BIND_EVENT_FUNC(AnimationSystem::OnAnimationEvent));
    }

    inline auto SetOriginalTexture(Ref<SubTexture2D> origTexture, Entity e) -> void
    {
        originalTextures_[e] = origTexture;
    }

private:
    auto OnAnimationEvent(AnimationEvent& e) -> bool
    {
        static Coordinator* coordinator = Coordinator::Instance(); 

        if (e.GetAnimationType() != GetEnumValue<T>())
            return false;

        Entity entity = e.GetEntityAffected(); 

        auto& animation = coordinator->GetComponent<T>(entity); 
        animation.Enabled = e.IsAnimationEnabled();

        auto& spriteRenderer = coordinator->GetComponent<SpriteRendererComponent>(entity); 

        if (!e.IsAnimationEnabled())
        {
            // Reset everything back to what it was.
            spriteIterators_[entity] = 0;
            frameCounters_[entity] = 0;

            // NOTE - If the original texture was nothing, then this will set a different texture.
            spriteRenderer.Texture = originalTextures_[entity];
        }
        else
        {
            // Cache the texture so we can restore it back when the animation is cancelled/stopped.
            if (!originalTextures_[entity])
                originalTextures_[entity] = spriteRenderer.Texture;

            spriteRenderer.Texture = animation.Animation.SpriteTextures[0];
        }

        return true;
    }

    array<size_t, MaxEntities> spriteIterators_{};
    array<size_t, MaxEntities> frameCounters_{};
    array<Ref<SubTexture2D>, MaxEntities> originalTextures_{};
};

using RunningAnimationSystem = AnimationSystem<RunningAnimationComponent>;
using SwingingAnimationSystem = AnimationSystem<SwingingAnimationComponent>;

class SmoothCameraFollowSystem : public System 
{
public:
    auto GetCalculatedPosition(Timestep ts) -> glm::vec3;

    float damping = 0.1f;
private:
    glm::vec3 calculatedPosition_;
};

