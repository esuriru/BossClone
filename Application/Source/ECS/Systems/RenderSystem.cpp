#include "RenderSystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Game/Tilemap.h"

#include "Renderer/Renderer2D.h"

#define CC_TILEMAP_FLIP

static Coordinator* coordinator = Coordinator::Instance();
auto SpriteRenderSystem::Update(Timestep ts) -> void
{
    for (auto& e : entities)
    {
        auto& transform = coordinator->GetComponent<TransformComponent>(e);
        auto& sprite = coordinator->GetComponent<SpriteRendererComponent>(e);

        Renderer2D::DrawSprite(transform.GetTransformMatrix(), sprite);
    }
}

auto TilemapRenderSystem::Update(Timestep ts) -> void
{
    for (auto& e : entities)
    {
        auto& transform = coordinator->GetComponent<TransformComponent>(e);
        auto& tile = coordinator->GetComponent<TilemapComponent>(e);

        const auto& transformMatrix = transform.GetTransformMatrix();

        for (int i = 0; i < TilemapData::TILEMAP_MAX_Y_LENGTH; ++i)
        {
            for (int j = 0; j < TilemapData::TILEMAP_MAX_X_LENGTH; ++j)
            {
                if (tile.MapData[i][j] == 0)
                    continue;

                glm::mat4 model = glm::translate(glm::mat4(1.0f), 
                    glm::vec3(j * tile.TileSize.x,
#ifdef CC_TILEMAP_FLIP
                        TilemapData::TILEMAP_MAX_Y_LENGTH - (i * tile.TileSize.y),
#else
                    i * tile.TileSize.y
#endif
                    0)) * transformMatrix;

                Renderer2D::DrawQuad(
                    model,
                    tile.SubTextureMap[tile.MapData[i][j]],
                    tile.TilingFactor,
                    tile.Colour);

            }
        }
    }
}
