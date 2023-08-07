#include "BatSystem.h"
#include "Events/ApplicationEvent.h"
#include "Core/Log.h"
#include "ECS/Systems/PlayerSystem.h"
#include "ECS/Component.h"
#include "Game/Tilemap.h"
#include <utility>

static Coordinator* coordinator = Coordinator::Instance();

BatSystem::BatSystem()
    : stateMachine_(this) 
{
}

void BatSystem::Update(Timestep ts)
{
    Entity e = *(entities.begin());

    if (!hasEnabledAnimation_)
    {
        hasEnabledAnimation_ = true;
        AnimationEvent event(Animation::AnimationType::Flying, e, true);
        eventCallback(event);
    }

    stateMachine_.Update(ts);

    Entity player = *(playerSystem->entities.begin());
    Entity tilemapEnt = coordinator->GetComponent<PhysicsQuadtreeComponent>(e).LastKnownTilemap;

    auto& bat = coordinator->GetComponent<BatComponent>(e);
    if (tilemapEnt == 0) return;
    auto& tilemap = coordinator->GetComponent<TilemapComponent>(tilemapEnt);
    auto& tilemapTransform = coordinator->GetComponent<TransformComponent>(tilemapEnt);
    auto& tilemapPos = tilemapTransform.Position; 
    auto& playerPos = coordinator->GetComponent<TransformComponent>(player).Position;
    auto& batPos = coordinator->GetComponent<TransformComponent>(e).Position;

    std::pair<size_t, size_t> playerIndices = std::make_pair(static_cast<size_t>((playerPos.x - tilemapPos.x + tilemap.TileSize.x * 0.5f) / tilemap.TileSize.x),
        static_cast<size_t>((playerPos.y - tilemapPos.y + tilemap.TileSize.y * 0.5f) / tilemap.TileSize.y));

    std::pair<size_t, size_t> batIndices = std::make_pair(static_cast<size_t>((playerPos.x - tilemapPos.x + tilemap.TileSize.x * 0.5f) / tilemap.TileSize.x),
        static_cast<size_t>((playerPos.y - tilemapPos.y + tilemap.TileSize.y * 0.5f) / tilemap.TileSize.y));

    auto path = tilemap.Pathfind(glm::vec2(playerIndices.first, playerIndices.second), glm::vec2(batIndices.first, batIndices.second), Heuristic::Euclidean, 10);

    bool firstPosition = true;
    glm::vec2 pathPoint;
    for (const auto& coord : path)
    {
        pathPoint = coord;
        if (firstPosition)
        {
            bat.Destination = glm::vec3(pathPoint.x * tilemap.TileSize.x, pathPoint.y * tilemap.TileSize.y, 0);
            bat.Direction = glm::normalize(glm::vec3(pathPoint, 0) - glm::vec3(batIndices.first, batIndices.second, 0));
            firstPosition = false;
        }
        else
        {
            if (glm::normalize(glm::vec3(pathPoint, 0) - glm::vec3(batIndices.first, batIndices.second, 0)) == bat.Direction)
            {
                bat.Destination = glm::vec3(pathPoint.x * tilemap.TileSize.x, pathPoint.y * tilemap.TileSize.y, 0);
            }
            else
                break;
        }
    }

}
