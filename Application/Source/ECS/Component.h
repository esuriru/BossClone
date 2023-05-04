#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <set>

#include "Renderer/Texture.h"
#include "Renderer/SubTexture2D.h"

// NOTE - Components are mainly for storing data. All the logic processes will be done in Systems.

struct TransformComponent
{
	glm::vec3 Position{};
	glm::vec3 Rotation = glm::vec3(0.0f, 0.f, 0.f); 
	glm::vec3 Scale = { 1, 1, 1 };

	TransformComponent() = default;
	TransformComponent(const glm::vec3& pos)
		: Position(pos) {};
    
    TransformComponent(const glm::vec3& pos, const glm::vec3& rot)
        : Position(pos), Rotation(rot) {}

    TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : Position(pos), Rotation(rot), Scale(scl) {}

    glm::mat4 GetTransformMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), Position)
            * glm::mat4_cast(glm::quat(Rotation))
            * glm::scale(glm::mat4(1.0f), Scale);
    }
};

struct TagComponent
{
    std::string Tag;
};

struct SpriteRendererComponent
{
    glm::vec4 Colour { 1.0f, 1.0f, 1.0f, 1.0f };
    Ref<SubTexture2D> Texture = nullptr;
    float TilingFactor = 1.0f;

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const glm::vec4& colour)
        : Colour(colour) {}
    SpriteRendererComponent(const Ref<SubTexture2D>& texture)
        : Texture(texture) {}
};

struct TileRendererComponent
{
    glm::vec4 Colour { 1.0f, 1.0f, 1.0f, 1.0f };
    Ref<SubTexture2D> Texture = nullptr;
    float TilingFactor = 1.0f;

    TileRendererComponent() = default;
    TileRendererComponent(const glm::vec4& colour)
        : Colour(colour) {}
    TileRendererComponent(const Ref<SubTexture2D>& texture)
        : Texture(texture) {}
};

struct Animation
{
    enum AnimationType
    {
        None = 0,
        Running,
    };
    std::vector<size_t> AnimationIndices;
    std::vector<Ref<SubTexture2D>> SpriteTextures;
    uint32_t FramesBetweenTransition = 8;
};

struct RunningAnimationComponent 
{
    RunningAnimationComponent() = default;

    Animation Animation;  
    bool Enabled = false;
};