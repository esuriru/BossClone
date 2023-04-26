#pragma once
#include "ECS/System.h"
#include "Core/Timestep.h"
#include <glm/glm.hpp>

#include "PhysicsComponent.h"

namespace Physics
{
    constexpr float GravityAccelerationScalar = -9.8f;
    constexpr glm::vec2 GravityAccelerationVector = { 0.f, GravityAccelerationScalar };
}

class ActiveTilemapSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

};

class PhysicsSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

    Entity tilemapEntity;
private:
    Timestep currentTimestep_;

    auto AddForce(RigidBody2DComponent& rigidbody, const glm::vec2& force, Physics::ForceMode mode = Physics::ForceMode::Force) -> void;
    auto TransformPositionToTilemapLocal(const glm::vec2& worldPos) -> glm::vec2;

};