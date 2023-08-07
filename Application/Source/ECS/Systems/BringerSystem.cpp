#include "BringerSystem.h"
#include <glm/gtx/norm.hpp>
#include "Events/ApplicationEvent.h"
#include <glm/gtx/string_cast.hpp>

static Coordinator* coordinator = Coordinator::Instance();

BringerSystem::BringerSystem()
    : hasJumped_(false)
    , chaseRange_(13300.0f)
    , currentState_(IDLE)
    , hasAttacked_(false)
    , timer_(0.0f)
    , toStopAnimation_(false)
{

}

void BringerSystem::Update(Timestep ts)
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

bool BringerSystem::OnAnimationSpriteChangeEvent(AnimationSpriteChangeEvent &event)
{
    if (entities.find(event.GetEntityAffected()) == entities.end())
        return false;
    
    constexpr glm::vec3 size = { 140, 93, 1 };
    if (event.GetAnimationType() != Animation::AnimationType::Swinging) return false;
    auto& animation = coordinator->GetComponent<SwingingAnimationComponent>(event.GetEntityAffected()).Animation;
    static glm::vec3 ppos;
    if (event.GetSpriteIndex() == animation.AnimationIndices[animation.AnimationIndices.size() - 2])
    {
        Entity player = *(playerSystem->entities.begin());
        ppos = coordinator->GetComponent<TransformComponent>(player).Position;
    }
    if (event.GetSpriteIndex() == animation.AnimationIndices.back())
    {
        hasJumped_ = false;
        affected_ = event.GetEntityAffected();
        toStopAnimation_ = true;

        Entity player = *(playerSystem->entities.begin());
        auto& transform = coordinator->GetComponent<TransformComponent>(event.GetEntityAffected());
        auto& pos = transform.Position;
        auto& playerPos = coordinator->GetComponent<TransformComponent>(player).Position;

        Entity m = coordinator->GetComponent<ReferenceComponent>(event.GetEntityAffected()).RefEntity;
        auto& col = coordinator->GetComponent<BoxCollider2DComponent>(m);
        auto& physics = coordinator->GetComponent<PhysicsQuadtreeComponent>(m);
        physics.Active = true;
        auto& t = coordinator->GetComponent<TransformComponent>(m);
        t.Scale = size;
        constexpr glm::vec3 offset = { 0, 20, 0 };
        t.Position = ppos + offset;

        // coordinator->GetComponent<TransformComponent>(m).Scale
        currentState_ = (glm::distance2(pos, playerPos) < chaseRange_) ? CHASE : IDLE;
        timer_ = 0.0f;
        hasAttacked_ = false;

        return true;
    }
    else if (event.GetSpriteIndex() == animation.AnimationIndices[1])
    {
        Entity m = coordinator->GetComponent<ReferenceComponent>(event.GetEntityAffected()).RefEntity;
        auto& physics = coordinator->GetComponent<PhysicsQuadtreeComponent>(m);
        physics.Active = false;
        auto& t = coordinator->GetComponent<TransformComponent>(m);
        t.Scale = {0, 0, 0};
        return true;
    }
    return false;
}

void BringerSystem::StateUpdate(Entity e, Timestep ts)
{
    Entity player = *(playerSystem->entities.begin());
    auto& nightborne = coordinator->GetComponent<BringerComponent>(e);
    auto& transform = coordinator->GetComponent<TransformComponent>(e);
    auto& pos = transform.Position;
    auto& playerPos = coordinator->GetComponent<TransformComponent>(player).Position;
    auto& rigidbody = coordinator->GetComponent<RigidBody2DComponent>(e);
    static const float attackRange = 2900.0f;
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
            else
            {
                Entity m = coordinator->GetComponent<ReferenceComponent>(e).RefEntity;
                auto& physics = coordinator->GetComponent<PhysicsQuadtreeComponent>(m);
                physics.Active = false;
                auto& t = coordinator->GetComponent<TransformComponent>(m);
                t.Scale = {0, 0, 0};

            }
            break;

        case CHASE:
            if (glm::distance2(pos, playerPos) < chaseRange_)
            {
                glm::vec2 direction = glm::distance2(pos, playerPos) < attackRange ? pos - playerPos : playerPos - pos; 
                rigidbody.LinearVelocity.x = glm::sign(direction.x) * nightborne.Speed;
                // rigidbody.LinearVelocity.x = -40; 
                timer_ += ts;
                CC_TRACE(glm::to_string(rigidbody.LinearVelocity));
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

}
