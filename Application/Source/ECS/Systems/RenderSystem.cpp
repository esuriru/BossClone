#include "RenderSystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#include "Renderer/Renderer2D.h"

static Coordinator* coordinator = Coordinator::Instance();
auto SpriteRenderSystem::Update(Timestep ts) -> void
{
    for (auto& e : entities)
    {
        auto& transform = coordinator->GetComponent<TransformComponent>(e);
        auto& sprite = coordinator->GetComponent<SpriteRendererComponent>(e);

        // Calculate the transform matrix.
        
        glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), transform.Position)
                                    * glm::mat4_cast(glm::quat(transform.Rotation))
                                    * glm::scale(glm::mat4(1.0f), transform.Scale);

        Renderer2D::DrawSprite(transformMatrix, sprite);
    }
}

auto TileRenderSystem::Update(Timestep ts) -> void
{
    for (auto& e : entities)
    {
        auto& transform = coordinator->GetComponent<TransformComponent>(e);
        auto& tile = coordinator->GetComponent<TileRendererComponent>(e);

        // Calculate the transform matrix.
        
        glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), transform.Position)
                                    * glm::mat4_cast(glm::quat(transform.Rotation))
                                    * glm::scale(glm::mat4(1.0f), transform.Scale);

        if (tile.Texture)
            Renderer2D::DrawQuad(transformMatrix, tile.Texture, tile.TilingFactor, tile.Colour);
        else
            Renderer2D::DrawQuad(transformMatrix, tile.Colour);
    }
}
