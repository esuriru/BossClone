#pragma once

#include <glm/glm.hpp>
#include "Core/Timestep.h"

namespace Physics
{
    enum class ForceMode
    {
        Force = 0,
        Acceleration,
        Impulse,
        VelocityChange
    };

    enum class RigidBodyType
    {
        Static = 0,
        Kinematic,
        Dynamic,
    };
}

struct PlayerController2DComponent
{
    float HorizontalForce = 50.f;
    float JumpForce = 500.f; 
};

struct RigidBody2DComponent
{
public:
    float Restitution = 0.f;
    Physics::RigidBodyType BodyType = Physics::RigidBodyType::Dynamic;
    glm::vec2 LinearVelocity = glm::vec2(0.0f);

    inline auto GetMass() const -> float
    {
        return mass_;
    }

    inline auto SetMass(float mass) -> void
    {
        mass_ = mass;
        inverseMass_ = 1.f / mass;
    }

    inline auto GetInverseMass() const -> float 
    {
        return inverseMass_;
    }
private:
    float mass_ = 1.0f;
    float inverseMass_ = 1.0f;

};

struct BoxCollider2DComponent
{
    glm::vec2 Offset = glm::vec2(0.f);
    glm::vec2 Extents = glm::vec2(0.5f);

    BoxCollider2DComponent(const glm::vec2& offset, const glm::vec2& extents)
        : Offset(offset), Extents(extents) {}
    BoxCollider2DComponent() = default;
};