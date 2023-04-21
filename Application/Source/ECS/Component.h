#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// NOTE - Components are mainly for storing data. All the logic processes will be done in Systems.

struct Transform
{
	glm::vec3 position{};
	glm::quat rotation = glm::quat(1.0f, 0.f, 0.f, 0.f); 
	glm::vec3 scale = { 1, 1, 1 };

	Transform() = default;
	Transform(const glm::vec3& pos)
		: position(pos) {};
};