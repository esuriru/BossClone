#include "PlayerSystem.h"

#include "Core/Core.h"
#include "Core/KeyCodes.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Utils/Input.h"

#include "Events/ApplicationEvent.h"
#include "Events/EventDispatcher.h"

#include <glm/glm.hpp>
#include <glm/common.hpp>

static Coordinator* coordinator = Coordinator::Instance();

PlayerSystem::PlayerSystem()
{
}

auto PlayerSystem::Update(Timestep ts) -> void
{
    if (entities.empty()) return;
    CC_ASSERT(entities.size() <= 1, "There should be no more than one player.");

    static Input* input = Input::Instance();

    constexpr float step = 1 / 50.f;
    static float accumulator = 0.f;

    accumulator += glm::min(static_cast<float>(ts), 0.25f);
        
    static bool wantsToDash = false;
    static bool wantsToJump = false;
    if (input->IsKeyPressed(Key::F) && currentDashCooldown_ == 0)
        wantsToDash = true;
    if (input->IsKeyPressed(Key::Space))
        wantsToJump = true;


    while (accumulator >= step) 
    {
        for (auto& e : entities)
        {
            auto& transform = coordinator->GetComponent<TransformComponent>(e);
            auto& rigidbody = coordinator->GetComponent<RigidBody2DComponent>(e);
            auto& player_controller = coordinator->GetComponent<PlayerController2DComponent>(e);

            if (input->IsKeyDown(Key::J))
            {
                const float accelerationScalar = physicsSystem->onGroundBitset.test(e) ?
                    player_controller.AccelerationScalar :
                        Physics::AirSpeedMultiplier * player_controller.AccelerationScalar;
                const float maxSpeed = physicsSystem->onGroundBitset.test(e) ?
                    player_controller.MaxHorizontalSpeed : 
                        Physics::AirSpeedMultiplier * player_controller.MaxHorizontalSpeed;
                rigidbody.LinearVelocity.x = (1 - accelerationScalar) * rigidbody.LinearVelocity.x
                    + accelerationScalar * -maxSpeed; 

                if (!runningBitset_.test(e))
                {
                    AnimationEvent event(Animation::AnimationType::Running, e, true);
                    eventCallback(event);
                    runningBitset_.set(e, true);
                }
                transform.Scale.x = -fabs(transform.Scale.x);
                player_controller.IsFacingRight = false;
            }
            else if (input->IsKeyDown(Key::L))
            {
                const float accelerationScalar = physicsSystem->onGroundBitset.test(e) ?
                    player_controller.AccelerationScalar :
                        Physics::AirSpeedMultiplier * player_controller.AccelerationScalar;
                const float maxSpeed = physicsSystem->onGroundBitset.test(e) ?
                    player_controller.MaxHorizontalSpeed : 
                        Physics::AirSpeedMultiplier * player_controller.MaxHorizontalSpeed;
                rigidbody.LinearVelocity.x = (1 - accelerationScalar) * rigidbody.LinearVelocity.x
                    + accelerationScalar * maxSpeed; 

                if (!runningBitset_.test(e))
                {
                    AnimationEvent event(Animation::AnimationType::Running, e, true);
                    eventCallback(event);
                    runningBitset_.set(e, true);
                }
                transform.Scale.x = fabs(transform.Scale.x);
                player_controller.IsFacingRight = true;
            }
            else
            {
                if (runningBitset_.test(e))
                {
                    runningBitset_.set(e, false);
                    AnimationEvent event(Animation::AnimationType::Running, e, false);
                    eventCallback(event);
                }
            }

            if (input->IsKeyDown(Key::K))
            {
                if (physicsSystem->onPlatformBitset.test(e))
                {
                    transform.Position.y -= Physics::PlatformCollisionDetectionThreshold + 1.0f;
                }
            }

            if (currentDashCooldown_ > 0)
                --currentDashCooldown_;

            bool hasJumped = false;
            if (wantsToJump && physicsSystem && physicsSystem->onGroundBitset.test(e) && rigidbody.LinearVelocity.y <= 0.0f)
            {
                wantsToJump = false;
                PhysicsSystem::AddForce(rigidbody, glm::vec2(0, player_controller.JumpForce), step, Physics::ForceMode::Impulse);
                hasJumped = true;
            }
            if (wantsToDash)
            {
                wantsToDash = false;
                currentDashCooldown_ += dashCooldownFrames_; 
                rigidbody.LinearVelocity.x = !hasJumped && physicsSystem->onGroundBitset.test(e) ? 
                    (-1 + 2 * static_cast<int>(player_controller.IsFacingRight)) * player_controller.MaxHorizontalSpeed : 
                    (-1 + 2 * static_cast<int>(player_controller.IsFacingRight)) * player_controller.MaxHorizontalSpeed * Physics::AirSpeedMultiplier;
            }

            // CC_TRACE("Player horizontal speed: ", rigidbody.LinearVelocity.x);

            // Using the item that the player currently holds.
            auto& inventory = coordinator->GetComponent<InventoryComponent>(e);    
            if (input->GetMouseButtonDown(0))
            {
                if (!mouseDown_)
                {
                    WeaponUseEvent event(e, inventory.CurrentlyHolding, true, player_controller.IsFacingRight); 
                    eventCallback(event);
                    mouseDown_ = true;
                }
            }
            else
            {
                if (mouseDown_)
                {
                    mouseDown_ = false;
                    WeaponUseEvent event(e, inventory.CurrentlyHolding, false, player_controller.IsFacingRight); 
                    eventCallback(event);
                }
            }

        }
        accumulator -= step;
    }
}

auto PlayerSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<CollisionEvent>(CC_BIND_EVENT_FUNC(PlayerSystem::OnCollisionEvent));
    dispatcher.Dispatch<KeyPressedEvent>(CC_BIND_EVENT_FUNC(PlayerSystem::OnKeyPressedEvent));
}

auto PlayerSystem::OnCollisionEvent(CollisionEvent &e) -> bool
{
    const auto& collision = e.GetCollision();

    bool originPlayer = false;
    Entity playerEntity;

    auto otherEntityQuery = entities.find(collision.OtherEntity);
    auto originEntityQuery = entities.find(e.GetOriginEntity());
    if (otherEntityQuery == entities.end())
    {
        if (originEntityQuery == entities.end())
            return false; 
        else
            originPlayer = true;
    }
    else
    {
        if (originEntityQuery == entities.end())
            originPlayer = false;
        else 
            return false;
    }

    Entity targetEntity;
    if (originPlayer)
    {
        playerEntity = e.GetOriginEntity();
        targetEntity = collision.OtherEntity;
    }
    else
    {
        playerEntity = collision.OtherEntity;
        targetEntity = e.GetOriginEntity();
    }

    // Send a pickup event.
    PickupEvent pickup(playerEntity, targetEntity, e.GetCollision());
    eventCallback(pickup);

    return false;
}

constexpr auto KeyToInventorySlot(int key) -> size_t
{
    return (key - 49);
}

auto PlayerSystem::OnKeyPressedEvent(KeyPressedEvent &e) -> bool
{
    // NOTE - There should be only one player, or this will not work.
    auto& inventory = coordinator->GetComponent<InventoryComponent>(*(entities.begin()));
    size_t slot = KeyToInventorySlot(e.GetKeyCode());
    constexpr uint32_t max_slot = InventorySize - 1;
    if (inventory.Items.empty() || slot > max_slot || slot > (inventory.Items.size() - 1)) return false;
    inventory.CurrentlyHolding = inventory.Items.at(slot);
    CC_TRACE("Currently holding: ", inventory.CurrentlyHolding);
    for (int i = 0; i < inventory.Items.size(); ++i)
    {
        CC_TRACE("[", i, "] : ", inventory.Items[i]);
    }
    return false;
}
