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
    float HorizontalForce = 5.f;
    float JumpForce = 5.f; 
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