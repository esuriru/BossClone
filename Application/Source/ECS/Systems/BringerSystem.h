#pragma once
#include "ECS/System.h"
#include "Core/Timestep.h"
#include "PlayerSystem.h"
#include "Core/Core.h"
#include "Core/Window.h"

class BringerSystem : public System
{
public:
    BringerSystem();

    void Update(Timestep ts);
    Ref<PlayerSystem> playerSystem;

    inline auto OnEvent(Event& e) -> void
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<AnimationSpriteChangeEvent>(CC_BIND_EVENT_FUNC(BringerSystem::OnAnimationSpriteChangeEvent));
    }

    EventCallback eventCallback;

private:
    bool OnAnimationSpriteChangeEvent(AnimationSpriteChangeEvent& event);

    void StateUpdate(Entity e, Timestep ts);

    enum State
    {
        IDLE,
        CHASE,
        ATTACK,
    };

    const float chaseRange_;
    bool hasJumped_;
    bool hasAttacked_;
    float timer_;
    bool toStopAnimation_;
    Entity affected_;

    State currentState_;

};