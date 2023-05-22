#include "RenderSystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Game/Tilemap.h"
#include "Core/Core.h"

#include "Utils/Util.h"

#include "Renderer/Renderer2D.h"
#include "Events/EventDispatcher.h"
#include "Events/ApplicationEvent.h"

#if 0
    #define CC_TILEMAP_FLIP
#endif

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
                if (tile.MapData[i][j].TextureIndex == 0)
                    continue;

                glm::mat4 model = transformMatrix * glm::translate(glm::mat4(1.0f), 
                    glm::vec3(
                        // (j - (0.5f * TilemapData::TILEMAP_MAX_X_LENGTH)) * tile.TileSize.x,
                        j * tile.TileSize.x,
#ifdef CC_TILEMAP_FLIP
                        // ((TilemapData::TILEMAP_MAX_Y_LENGTH - i) - 0.5f * TilemapData::TILEMAP_MAX_Y_LENGTH) * tile.TileSize.y,
                        (TilemapData::TILEMAP_MAX_Y_LENGTH - i) * tile.TileSize.y,
#else
                        i * tile.TileSize.y,
#endif
                    0)) * glm::scale(glm::mat4(1.0f), glm::vec3(tile.TileSize, 0));

                Renderer2D::DrawQuad(
                    model,
                    tile.SubTextureMap[tile.MapData[i][j].TextureIndex],
                    tile.TilingFactor,
                    tile.Colour);

            }
        }
    }
}

auto SmoothCameraFollowSystem::GetCalculatedPosition(Timestep ts) -> glm::vec3
{
    constexpr float step = 1 / CC_FIXED_UPDATE_FRAME_RATE;
    static float accumulator = 0.f;

    accumulator += glm::min(static_cast<float>(ts), 0.25f);

    while (accumulator >= step)
    {
        CC_ASSERT(entities.size() <= 1, "There should be only one player.");
        auto& player = *entities.begin();

        auto& transform = coordinator->GetComponent<TransformComponent>(player);
        calculatedPosition_.z = transform.Position.z;

        calculatedPosition_ = Utility::Lerp(calculatedPosition_, transform.Position, damping);

        accumulator -= step;
    }
    return calculatedPosition_;
}

