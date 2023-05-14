#include "PlayerSystem.h"

#include "Core/Core.h"
#include "Core/KeyCodes.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Utils/Input.h"

#include "Events/ApplicationEvent.h"

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

    while (accumulator >= step) 
    {
        for (auto& e : entities)
        {
            auto& transform = coordinator->GetComponent<TransformComponent>(e);
            auto& rigidbody = coordinator->GetComponent<RigidBody2DComponent>(e);
            auto& player_controller = coordinator->GetComponent<PlayerController2DComponent>(e);

            if (input->IsKeyDown(Key::J))
            {
                // PhysicsSystem::AddForce(rigidbody,
                //     glm::vec2(physicsSystem->onGroundBitset.test(e) ? -player_controller.HorizontalForce : -player_controller.AirHorizontalForce, 0),
                //     step);
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
            }
            else if (input->IsKeyDown(Key::L))
            {
                // PhysicsSystem::AddForce(rigidbody,
                //     glm::vec2(physicsSystem->onGroundBitset.test(e) ? player_controller.HorizontalForce : player_controller.AirHorizontalForce, 0),
                //     step);
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

            if (input->IsKeyDown(Key::Space) && physicsSystem && physicsSystem->onGroundBitset.test(e) && rigidbody.LinearVelocity.y <= 0.0f)
            {
                PhysicsSystem::AddForce(rigidbody, glm::vec2(0, player_controller.JumpForce), step, Physics::ForceMode::Impulse);
            }

            if (input->IsKeyDown(Key::K))
            {
                if (physicsSystem->onPlatformBitset.test(e))
                {
                    transform.Position.y -= Physics::PlatformCollisionDetectionThreshold + 1.0f;
                }
            }

            // Using the item that the player currently holds.
            auto& inventory = coordinator->GetComponent<InventoryComponent>(e);    
            if (input->GetMouseButtonDown(0))
            {
                if (!mouseDown_)
                {
                    WeaponUseEvent event(e, inventory.CurrentlyHolding, true); 
                    eventCallback(event);
                    mouseDown_ = true;
                }
            }
            else
            {
                if (mouseDown_)
                {
                    mouseDown_ = false;
                    WeaponUseEvent event(e, inventory.CurrentlyHolding, false); 
                    eventCallback(event);
                }
            }
        }
        accumulator -= step;
    }
}
