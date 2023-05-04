#include "RenderSystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Game/Tilemap.h"
#include "Core/Core.h"

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
                if (tile.MapData[i][j].Type == Tile::TileType::Empty)
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

auto RunningAnimationSystem::Update(Timestep ts) -> void
{
    for (auto& e : entities)
    {
        auto& spriteRenderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        auto& runningAnimation = coordinator->GetComponent<RunningAnimationComponent>(e);

        if (!runningAnimation.Enabled)
        {
            return;
        }

        auto& animation = runningAnimation.Animation;
        if (animation.AnimationIndices.empty())
        {
            return;
        }

        spriteRenderer.Texture = animation.SpriteTextures[animation.AnimationIndices[spriteIterators_[e]]];

        if (++frameCounters_[e] >= animation.FramesBetweenTransition)
        {
            // If the sprite shown is already the last sprite, go back to the first loop.
            if (animation.AnimationIndices.size() - 1 == spriteIterators_[e])
            {
                spriteIterators_[e] = 0;    
            }
            else
            {
                ++spriteIterators_[e];
            }
            frameCounters_[e] -= animation.FramesBetweenTransition;
        }
    }
}

auto RunningAnimationSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<AnimationEvent>(CC_BIND_EVENT_FUNC(RunningAnimationSystem::OnAnimationEvent));
}

auto RunningAnimationSystem::OnAnimationEvent(AnimationEvent &e) -> bool
{
    if (e.GetAnimationType() != Animation::AnimationType::Running)
        return false;

    Entity entity = e.GetEntityAffected(); 

    auto& runningAnimation = coordinator->GetComponent<RunningAnimationComponent>(entity); 
    runningAnimation.Enabled = e.IsAnimationEnabled();

    auto& spriteRenderer = coordinator->GetComponent<SpriteRendererComponent>(entity); 

    if (!e.IsAnimationEnabled())
    {
        // Reset everything back to what it was.
        spriteIterators_[entity] = 0;
        frameCounters_[entity] = 0;

        spriteRenderer.Texture = originalTextures_[entity];
    }
    else
    {
        // Cache the texture so we can restore it back when the animation is cancelled/stopped.
        originalTextures_[entity] = spriteRenderer.Texture;
    }

    return true;
}
