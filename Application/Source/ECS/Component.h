#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Renderer/Texture.h"

// NOTE - Components are mainly for storing data. All the logic processes will be done in Systems.

struct TransformComponent
{
	glm::vec3 Position{};
	glm::vec3 Rotation = glm::vec3(0.0f, 0.f, 0.f); 
	glm::vec3 Scale = { 1, 1, 1 };

	TransformComponent() = default;
	TransformComponent(const glm::vec3& pos)
		: Position(pos) {};
};

struct TagComponent
{
    std::string Tag;
};

struct SpriteRendererComponent
{
    glm::vec4 Colour { 1.0f, 1.0f, 1.0f, 1.0f };
    Ref<Texture2D> Texture; 
    float TilingFactor = 1.0f;

    SpriteRendererComponent(const glm::vec4& colour)
        : Colour(colour) {}
};