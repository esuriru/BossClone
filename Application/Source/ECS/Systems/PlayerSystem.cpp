#include "PlayerSystem.h"

#include "Core/Core.h"
#include "Core/KeyCodes.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Utils/Input.h"

#include <glm/glm.hpp>
#include <glm/common.hpp>

static Coordinator* coordinator = Coordinator::Instance();

auto PlayerSystem::Update(Timestep ts) -> void
{
    if (entities.empty()) return;

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
                PhysicsSystem::AddForce(rigidbody, glm::vec2(-player_controller.HorizontalForce, 0), step);
            else if (input->IsKeyDown(Key::L))
                PhysicsSystem::AddForce(rigidbody, glm::vec2(player_controller.HorizontalForce, 0), step);

            if (input->IsKeyDown(Key::Space) && physicsSystem && physicsSystem->onGroundBitset.test(e) && rigidbody.LinearVelocity.y <= 0.0f)
                PhysicsSystem::AddForce(rigidbody, glm::vec2(0, player_controller.JumpForce), step, Physics::ForceMode::Impulse);

            if (input->IsKeyDown(Key::K))
            {
                if (physicsSystem->onPlatformBitset.test(e))
                {
                    transform.Position.y -= Physics::PlatformCollisionDetectionThreshold + 1.0f;
                }
            }
        }
        accumulator -= step;
    }
}
