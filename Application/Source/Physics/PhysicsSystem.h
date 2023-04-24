#pragma once
#include "ECS/System.h"
#include "Core/Timestep.h"
#include <glm/glm.hpp>

namespace Physics
{
    constexpr float gravity_acceleration_scalar = -9.8;
    constexpr glm::vec3 gravity_acceleration_vector = { 0.f, gravity_acceleration_scalar, 0.f };
}

class PhysicsSystem : public System
{
public:
    auto Update(Timestep ts) -> void;
private:
    std::vector<Entity> tilemapEntities_;
};