#include "NightborneSystem.h"
#include <glm/gtx/norm.hpp>
#include "Events/ApplicationEvent.h"

static Coordinator* coordinator = Coordinator::Instance();

NightborneSystem::NightborneSystem()
    : hasJumped_(false)
    , chaseRange_(7300.0f)
    , currentState_(IDLE)
    , hasAttacked_(false)
    , timer_(0.0f)
    , toStopAnimation_(false)
{

}

void NightborneSystem::Update(Timestep ts)
{
    if (toStopAnimation_)
    {
        AnimationEvent animationEvent(Animation::AnimationType::Swinging, affected_, false);
        eventCallback(animationEvent);
        toStopAnimation_ = false;
    }
    for (auto& e : entities) 
    {
        StateUpdate(e, ts);
    }
}

bool NightborneSystem::OnAnimationSpriteChangeEvent(AnimationSpriteChangeEvent &event)
{
    if (entities.find(event.GetEntityAffected()) == entities.end())
        return false;
    
    if (event.GetAnimationType() != Animation::AnimationType::Swinging) return false;
    auto& animation = coordinator->GetComponent<SwingingAnimationComponent>(event.GetEntityAffected()).Animation;
    if (event.GetSpriteIndex() == animation.AnimationIndices.back())
    {
        hasJumped_ = false;
        affected_ = event.GetEntityAffected();
        toStopAnimation_ = true;
        Entity player = *(playerSystem->entities.begin());
        auto& transform = coordinator->GetComponent<TransformComponent>(event.GetEntityAffected());
        auto& pos = transform.Position;
        auto& playerPos = coordinator->GetComponent<TransformComponent>(player).Position;
        currentState_ = (glm::distance2(pos, playerPos) < chaseRange_) ? CHASE : IDLE;
        timer_ = 0.0f;
        hasAttacked_ = false;

        return true;
    }
    else if (event.GetSpriteIndex() == 8)
    {
        constexpr glm::vec2 offset = { 31, 0 };
        auto& transform = coordinator->GetComponent<TransformComponent>(event.GetEntityAffected());
        Entity m = coordinator->GetComponent<ReferenceComponent>(event.GetEntityAffected()).RefEntity;
        auto& col = coordinator->GetComponent<BoxCollider2DComponent>(m);
        // col.Offset = offset; 
        auto& physics = coordinator->GetComponent<PhysicsQuadtreeComponent>(m);
        physics.Active = true;
        auto& t = coordinator->GetComponent<TransformComponent>(m);
        t.Position = coordinator->GetComponent<TransformComponent>(event.GetEntityAffected()).Position + glm::vec3(glm::sign(transform.Scale.x) * offset.x, offset.y, 0);
    }
    else if (event.GetSpriteIndex() == 9)
    {
        Entity m = coordinator->GetComponent<ReferenceComponent>(event.GetEntityAffected()).RefEntity;
        auto& physics = coordinator->GetComponent<PhysicsQuadtreeComponent>(m);
        physics.Active = false;
    }
    return false;
}

void NightborneSystem::StateUpdate(Entity e, Timestep ts)
{
    Entity player = *(playerSystem->entities.begin());
    auto& nightborne = coordinator->GetComponent<NightborneComponent>(e);
    auto& transform = coordinator->GetComponent<TransformComponent>(e);
    auto& pos = transform.Position;
    auto& playerPos = coordinator->GetComponent<TransformComponent>(player).Position;
    auto& rigidbody = coordinator->GetComponent<RigidBody2DComponent>(e);
    static const float attackRange = 900.0f;
    static const float jumpForce = 8.0f;
    static const float attackTimer = 1.5f;

    switch (currentState_)
    {
        case IDLE:
            if (glm::distance2(pos, playerPos) < chaseRange_)
            {
                hasAttacked_ = false;
                currentState_ = CHASE;
            }
            CC_TRACE("idle");
            break;

        case CHASE:
            if (glm::distance2(pos, playerPos) < chaseRange_)
            {
                glm::vec2 direction = playerPos - pos; 
                rigidbody.LinearVelocity.x = glm::sign(direction.x) * nightborne.Speed;
                timer_ += ts;
                // CC_TRACE(timer_);
                if (timer_ >= attackTimer)
                {
                    timer_ = 0.0f;
                    currentState_ = ATTACK;
                }
            }
            else
            {
                currentState_ = IDLE;
                timer_ = 0.0f;
            }
            break;

        case ATTACK:
            if (!hasAttacked_)
            {
                glm::vec2 direction = playerPos - pos; 
                if (!hasJumped_ && direction.y > 0)
                {
                    hasJumped_ = true; 
                    rigidbody.LinearVelocity.y = jumpForce;
                }
                AnimationEvent animationEvent(Animation::AnimationType::Swinging, e, true);
                eventCallback(animationEvent);
                hasAttacked_ = true;
                CC_TRACE("attack");
            }
            timer_ = 0.0f;
            break;
    }

    transform.Scale.x = rigidbody.LinearVelocity.x > 0 ? fabs(transform.Scale.x) : -fabs(transform.Scale.x);
}
