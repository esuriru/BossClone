#include "PhysicsSystem.h"

#include "PhysicsComponent.h"
#include "ECS/Component.h"

#include "ECS/Coordinator.h"
#include "Game/Tilemap.h"
#include "Core/Core.h"
#include "Game/Tilemap.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>

#include "Utils/Util.h"

#include <limits>

static Coordinator* coordinator = Coordinator::Instance();

auto ActiveTilemapSystem::Update(Timestep ts) -> void
{
}

auto ActiveTilemapSystem::GetClosestTilemap(const glm::vec2 &position) -> Entity
{
    Entity tilemapEntity;

    float nearestDistance = std::numeric_limits<float>::max();
    for (auto& e : entities)
    {
        TransformComponent transform = coordinator->GetComponent<TransformComponent>(e);
        TilemapComponent tilemap = coordinator->GetComponent<TilemapComponent>(e);

        // NOTE - We want the centre of the tilemap such that it is equidistant from each other to allow only collision checking inside of that tilemap 'chunk'
        float distanceBetween = glm::distance2(GetTilemapCentre(tilemap, transform), position);
        if (nearestDistance > distanceBetween)
        {
            nearestDistance = distanceBetween;
            tilemapEntity = e;
        }
    }

    return tilemapEntity;
}

auto ActiveTilemapSystem::GetTilemapCentre(TilemapComponent &tilemap, TransformComponent &transform) const -> glm::vec2
{
    glm::vec2 position_vec2 = glm::vec2(transform.Position);
    return (position_vec2 + 0.5f * glm::vec2(TilemapData::TILEMAP_MAX_X_LENGTH * tilemap.TileSize.x, TilemapData::TILEMAP_MAX_Y_LENGTH * tilemap.TileSize.y));
}

auto PhysicsSystem::Update(Timestep ts) -> void
{
    if (entities.empty()) return;

    constexpr int8_t velocityIterations = 6;
    constexpr int8_t positionIterations = 2;

    constexpr float step = 1 / 50.f;
    static float accumulator = 0.f;

    accumulator += glm::min(static_cast<float>(ts), 0.25f);

    while (accumulator >= step) 
    {
        currentTimestep_ = step;
        for (auto& e : entities)
        {
            auto& transform = coordinator->GetComponent<TransformComponent>(e);
            auto& rigidbody = coordinator->GetComponent<RigidBody2DComponent>(e);
            auto& box_collider = coordinator->GetComponent<BoxCollider2DComponent>(e);

            // Add gravity.
            if (!onGroundBitset.test(e))
            {
                AddForce(rigidbody, Physics::GravityAccelerationVector, Physics::ForceMode::Acceleration);
            }
            else
            {
                rigidbody.LinearVelocity.x = 0.98f * rigidbody.LinearVelocity.x;
                // TODO - Maybe make it go all the way to 0 after it reaches a certain threshhold, do I need to introduce sleeping?
            }

            glm::vec2 position_vec2 = glm::vec2(transform.Position);
            glm::vec2 proposedPosition = position_vec2 + (rigidbody.LinearVelocity * static_cast<float>(step));

            // Tilemap collision

            // Broad phase chunking 
            tilemapEntity = tilemapSystem->GetClosestTilemap(transform.Position);
            if (tilemapEntity == 0)
                continue;

            auto& nearestTilemap = coordinator->GetComponent<TilemapComponent>(tilemapEntity);
            glm::vec3& tilemapPosition = coordinator->GetComponent<TransformComponent>(tilemapEntity).Position;

            float groundLevel = 0.f;
            bool onPlatform = false;
            auto tilemapCollisionDetectionResult = 
                CheckTilemapCollision(position_vec2, proposedPosition, box_collider, nearestTilemap, tilemapPosition, groundLevel, onPlatform);

            if (rigidbody.LinearVelocity.y <= 0.f && tilemapCollisionDetectionResult)
            {
                proposedPosition.y = groundLevel + box_collider.Extents.y - box_collider.Offset.y;
                rigidbody.LinearVelocity.y = 0.f;
                onGroundBitset.set(e, true);
                if (onPlatform)
                {
                    CC_TRACE("On platform");
                    onPlatformBitset.set(e, true);
                }
            }
            else
            {
                onGroundBitset.set(e, false);
                onPlatformBitset.set(e, false);
            }

            transform.Position = glm::vec3(proposedPosition, 0);
        }

        accumulator -= step;
    }
}

auto PhysicsSystem::CheckTilemapCollision(const glm::vec2 &oldPosition, const glm::vec2 &newPosition, const BoxCollider2DComponent &boxCollider,
    const TilemapComponent &tilemap, const glm::vec3 &tilemapPosition, float& groundLevel, bool& onPlatform) -> bool 
{
    glm::vec2 oldCentre = oldPosition + boxCollider.Offset;
    glm::vec2 newCentre = newPosition + boxCollider.Offset;

    const glm::vec2 bottomLeftOffset = boxCollider.Extents - glm::vec2(1, -1);

    glm::vec2 oldBottomLeft = oldCentre - bottomLeftOffset;

    glm::vec2 newBottomLeft = newCentre - bottomLeftOffset; 
    glm::vec2 newBottomRight = glm::vec2(newBottomLeft.x + boxCollider.Extents.x * 2.0f - 2.0f, newBottomLeft.y);

    size_t destinationY = GetTileIndexYAtWorldPoint(tilemapPosition, tilemap.TileSize, newBottomLeft.y);
    size_t fromY = glm::max(GetTileIndexYAtWorldPoint(tilemapPosition, tilemap.TileSize, oldBottomLeft.y) - 1, destinationY);
    float inverseDistInTiles = 1.f / glm::max(static_cast<int>(glm::abs(destinationY - fromY)), 1);

    // TODO - Early end when out of bounds.
    size_t index_x = 0;
    size_t index_y = 0;

    // NOTE - From the previous position, to the next position, go through all the possible tiles.
    for (size_t index_y = fromY; index_y >= destinationY; --index_y)
    {
        glm::vec2 bottomLeft = Utility::Lerp(newBottomLeft, oldBottomLeft, static_cast<float>(glm::abs(destinationY - index_y)) * inverseDistInTiles);
        glm::vec2 bottomRight = glm::vec2(bottomLeft.x + boxCollider.Extents.x * 2.0f - 2.0f, bottomLeft.y);
        // NOTE - Basically, scan from the bottom left to the bottom right of the AABB for tiles.
        for (glm::vec2 checkedTile = bottomLeft;; checkedTile.x += tilemap.TileSize.x)
        {
            checkedTile.x = glm::min(checkedTile.x, bottomRight.x);

            index_x = GetTileIndexXAtWorldPoint(tilemapPosition, tilemap.TileSize, checkedTile.x);

            groundLevel = static_cast<float>(index_y) * tilemap.TileSize.y + tilemap.TileSize.y * 0.5f + tilemapPosition.y;

            // Check if it is in bounds.
            if (index_x < 0 || index_x > TilemapData::TILEMAP_MAX_X_LENGTH - 1 || index_y < 0 || index_y > TilemapData::TILEMAP_MAX_Y_LENGTH - 1)
            {
                break;
            }

            auto& tileType = tilemap.MapData[index_y][index_x].Type;
            if (tileType == Tile::TileType::Solid)
            {
                onPlatform = false;
                return true;
            }
            else if (tileType == Tile::TileType::OneWay && fabs(checkedTile.y - groundLevel) <= Physics::PlatformCollisionDetectionThreshold)
            {
                onPlatform = true;  
            }

            if (checkedTile.x >= bottomRight.x)
            {
                if (onPlatform)
                    return true;
                break;
            }
        }
    }
    return false;
}

auto PhysicsSystem::AddForce(RigidBody2DComponent &rigidbody, const glm::vec2 &force, Physics::ForceMode mode) -> void
{
    PhysicsSystem::AddForce(rigidbody, force, static_cast<float>(currentTimestep_), mode);
}

auto PhysicsSystem::AddForce(RigidBody2DComponent &rigidbody, const glm::vec2 &force, float deltaTime, Physics::ForceMode mode) -> void
{
    if (rigidbody.BodyType == Physics::RigidBodyType::Kinematic) return;

    switch (mode)
    {
    case Physics::ForceMode::Acceleration:
        rigidbody.LinearVelocity += force * deltaTime * Physics::TimeScale;
        break;
    case Physics::ForceMode::Force:
        rigidbody.LinearVelocity += force * deltaTime * Physics::TimeScale * rigidbody.GetInverseMass();
        break;
    case Physics::ForceMode::Impulse:
        rigidbody.LinearVelocity += force * rigidbody.GetInverseMass();
        break;
    case Physics::ForceMode::VelocityChange:
        rigidbody.LinearVelocity += force;
        break;
    }
}

auto PhysicsSystem::TransformPositionToTilemapLocal(const glm::vec2 &worldPos) -> glm::vec2
{
    glm::vec3 tilemapPosition = coordinator->GetComponent<TransformComponent>(tilemapEntity).Position;
    glm::vec2 localPos = worldPos - glm::vec2(tilemapPosition.x, tilemapPosition.y);
    auto& tilemap = coordinator->GetComponent<TilemapComponent>(tilemapEntity);
    return (localPos + 0.5f * glm::vec2(tilemap.TileSize.x * TilemapData::TILEMAP_MAX_X_LENGTH, tilemap.TileSize.y * TilemapData::TILEMAP_MAX_Y_LENGTH));
}

auto PhysicsSystem::GetTileIndexXAtWorldPoint(const glm::vec3 &tilemapPosition, const glm::vec2 &tileSize, const float x) const -> size_t 
{
    return static_cast<size_t>((x - tilemapPosition.x + tileSize.x * 0.5f) / tileSize.x);
}

auto PhysicsSystem::GetTileIndexYAtWorldPoint(const glm::vec3 &tilemapPosition, const glm::vec2 &tileSize, const float y) const -> size_t 
{
    return static_cast<size_t>((y - tilemapPosition.y + tileSize.y * 0.5f) / tileSize.y);
}

auto PhysicsSystem::GetTileWorldPosition(const glm::vec3& tilemapWorldPosition, const glm::vec2& tileSize, size_t index_x, size_t index_y) const -> glm::vec2
{
    return (glm::vec2(index_x, index_y) * tileSize) + glm::vec2(tilemapWorldPosition);
}

auto PhysicsSystem::GetTileWorldPosition(const glm::vec3& tilemapWorldPosition, const glm::vec2& tileSize, glm::vec2 column_row) const -> glm::vec2
{
    return (column_row * tileSize) + glm::vec2(tilemapWorldPosition);
}
