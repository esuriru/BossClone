#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform
{
	glm::vec3 position{};
	glm::quat rotation = glm::quat(1.0f); 
	glm::vec3 scale = { 1, 1, 1 };

	Transform() = default;
	Transform(const glm::vec3& pos)
		: position(pos) {};


private:
	Transform* parent_;
	std::vector<Transform*> children_;
};