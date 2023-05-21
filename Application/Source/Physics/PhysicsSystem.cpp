#include "PhysicsSystem.h"

#include "PhysicsComponent.h"
#include "ECS/Component.h"

#include "Physics/Collision2D.h"

#include "ECS/Coordinator.h"
#include "Game/Tilemap.h"
#include "Core/Core.h"
#include "Game/Tilemap.h"

#include "Events/ApplicationEvent.h"
#include "Events/EventDispatcher.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>

#include <utility>

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

auto ActiveTilemapSystem::CheckCollisions() -> void
{
    for (auto& e : entities)
    {
        auto& tilemap = coordinator->GetComponent<TilemapComponent>(e);

        for (int y = 0; y < tilemap.VerticalAreasCount; ++y)
        {
            for (int x = 0; x < tilemap.HorizontalAreasCount; ++x)
            {
                auto& objects = tilemap.ObjectsInArea[x][y];
                for (int i = 0; i < static_cast<int>(objects.size()) - 1; ++i)
                {
                    auto& obj1 = objects[i];
                    for (int j = i + 1; j < static_cast<int>(objects.size()); ++j)
                    {
                        auto& obj2 = objects[j];

                        auto& physics_quadtree_comp1 = coordinator->GetComponent<PhysicsQuadtreeComponent>(obj1);
                        auto& physics_quadtree_comp2 = coordinator->GetComponent<PhysicsQuadtreeComponent>(obj2);


                        if (HasCollisionData(physics_quadtree_comp1, obj2) || HasCollisionData(physics_quadtree_comp2, obj1) ||
                            !physics_quadtree_comp1.Active || !physics_quadtree_comp2.Active)
                        {
                            continue;
                        } 


                        // NOTE - The design of the physics system is such that there will be a need for
                        // NOTE - each physics object to have both a box collider and a rigidbody.
                        auto& col1 = coordinator->GetComponent<BoxCollider2DComponent>(obj1);
                        auto pos1 = glm::vec2(coordinator->GetComponent<TransformComponent>(obj1).Position);
                        auto& rb1 = coordinator->GetComponent<RigidBody2DComponent>(obj1);
                        auto& col2 = coordinator->GetComponent<BoxCollider2DComponent>(obj2);
                        auto pos2 = glm::vec2(coordinator->GetComponent<TransformComponent>(obj2).Position);
                        auto& rb2 = coordinator->GetComponent<RigidBody2DComponent>(obj2);

                        glm::vec2 overlap;

                        if (PhysicsSystem::AABBTest(col1, pos1, col2, pos2, overlap)) 
                        {
                            Collision2D obj1_collision = Collision2D(obj2, overlap, rb1.LinearVelocity, rb2.LinearVelocity, pos1, pos2);
                            physics_quadtree_comp1.Collisions.emplace_back(obj1_collision);
                            physics_quadtree_comp2.Collisions.emplace_back(Collision2D(obj1, overlap, rb2.LinearVelocity, rb1.LinearVelocity, pos2, pos1));

                            CollisionEvent event(obj1, obj1_collision);
                            eventCallback(event);
                        }
                    }
                }
            }
        }
    }
}

auto ActiveTilemapSystem::HasCollisionData(const PhysicsQuadtreeComponent& pqc, Entity e) -> bool
{
    for (const auto& c : pqc.Collisions)
    {
        if (c.OtherEntity == e)
            return true;
    }
    return false;
}

auto PhysicsSystem::Update(Timestep ts) -> void
{
    if (entities.empty()) return;
    // CC_TRACE("Physics system entities size: ", entities.size());

    // constexpr int8_t velocityIterations = 6;
    // constexpr int8_t positionIterations = 2;

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

            bool entityOnGround = onGroundBitset.test(e);

            // Broad phase chunking 
            tilemapEntity = tilemapSystem->GetClosestTilemap(transform.Position);
            if (tilemapEntity == 0)
            {
                continue;
            }

            auto& nearestTilemap = coordinator->GetComponent<TilemapComponent>(tilemapEntity);
            glm::vec3& tilemapPosition = coordinator->GetComponent<TransformComponent>(tilemapEntity).Position;

            UpdateAreas(nearestTilemap, tilemapPosition, e, transform, box_collider);
            auto& physics_quadtree_component = coordinator->GetComponent<PhysicsQuadtreeComponent>(e);
            physics_quadtree_component.Collisions.clear();

            if (rigidbody.BodyType == Physics::RigidBodyType::Static)
                continue;

            // Add gravity.
            if (!entityOnGround)
            {
                AddForce(rigidbody, Physics::GravityAccelerationVector, Physics::ForceMode::Acceleration);
            }
            else
            {
                if (fabs(rigidbody.LinearVelocity.x) > glm::epsilon<float>())
                {
                    rigidbody.LinearVelocity.x = Physics::FrictionCoefficient * rigidbody.LinearVelocity.x;
                    if (fabs(rigidbody.LinearVelocity.x) < glm::epsilon<float>())
                    {
                        // Round it off.
                        rigidbody.LinearVelocity.x = 0;
                    }
                }
                // TODO - Maybe make it go all the way to 0 after it reaches a certain threshhold, do I need to introduce sleeping?
            }

            glm::vec2 position_vec2 = glm::vec2(transform.Position);
            glm::vec2 proposedPosition = position_vec2 + (rigidbody.LinearVelocity * static_cast<float>(step));

            // Tilemap collision

            bool resolved = false;
            float groundLevel = 0.f;
            bool onPlatform = false;

            float leftTileX = 0.f, rightTileX = 0.f;

            auto tilemapLeftCollisionDetectionResult = 
                CheckTilemapCollisionLeft(position_vec2, proposedPosition, box_collider,
                    nearestTilemap, tilemapPosition, leftTileX);
            if (rigidbody.LinearVelocity.x <= 0.f && tilemapLeftCollisionDetectionResult)
            {
                if ((position_vec2.x - box_collider.Extents.x + box_collider.Offset.x) >= leftTileX)
                {
                    proposedPosition.x = leftTileX + box_collider.Extents.x - box_collider.Offset.x;
                }
                rigidbody.LinearVelocity.x = glm::max(rigidbody.LinearVelocity.x, 0.f);
            }

            glm::vec2 newProposedPosition = proposedPosition + (rigidbody.LinearVelocity * static_cast<float>(step));

            auto tilemapRightCollisionDetectionResult = 
                CheckTilemapCollisionRight(proposedPosition, newProposedPosition, box_collider,
                    nearestTilemap, tilemapPosition, rightTileX);
            if (rigidbody.LinearVelocity.x >= 0.f && tilemapRightCollisionDetectionResult)
            {
                if ((position_vec2.x + box_collider.Extents.x + box_collider.Offset.x) <= rightTileX)
                {
                    proposedPosition.x = rightTileX - box_collider.Extents.x - box_collider.Offset.x;
                }
                rigidbody.LinearVelocity.x = glm::min(rigidbody.LinearVelocity.x, 0.f);
            }

            newProposedPosition = proposedPosition + (rigidbody.LinearVelocity * static_cast<float>(step));

            float ceilingLevel = 0.f;
            auto tilemapCeilingCollisionDetectionResult = 
                CheckTilemapCollisionCeiling(proposedPosition, newProposedPosition, box_collider,
                    nearestTilemap, tilemapPosition, ceilingLevel);
            if (rigidbody.LinearVelocity.y >= 0.f && tilemapCeilingCollisionDetectionResult)
            {
                proposedPosition.y = ceilingLevel - box_collider.Extents.y - box_collider.Offset.y - 1.0f;
                rigidbody.LinearVelocity.y = 0.f;
                onGroundBitset.set(e, false);
            }

            newProposedPosition = proposedPosition + (rigidbody.LinearVelocity * static_cast<float>(step));

            auto tilemapGroundCollisionDetectionResult = 
                CheckTilemapCollisionGround(proposedPosition, newProposedPosition, box_collider,
                    nearestTilemap, tilemapPosition, groundLevel, onPlatform);

            if (rigidbody.LinearVelocity.y <= 0.f && tilemapGroundCollisionDetectionResult)
            {
                proposedPosition.y = groundLevel + box_collider.Extents.y - box_collider.Offset.y;
                rigidbody.LinearVelocity.y = 0.f;
                resolved = true;
                onGroundBitset.set(e, true);
                if (onPlatform)
                {
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

        tilemapSystem->CheckCollisions();
        accumulator -= step;
    }
}

auto PhysicsSystem::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<OnEntityDestroyedEvent>(CC_BIND_EVENT_FUNC(PhysicsSystem::OnOnEntityDestroyedEvent));
}

auto PhysicsSystem::OnOnEntityDestroyedEvent(OnEntityDestroyedEvent &e) -> bool
{
    if (entities.find(e.GetDestroyedEntity()) != entities.end())
        RemoveEntityFromQuadtree(e.GetDestroyedEntity());
    return false;
}

auto PhysicsSystem::CheckTilemapCollisionGround(const glm::vec2 &oldPosition,
                                                const glm::vec2 &newPosition,
                                                const BoxCollider2DComponent &boxCollider,
                                                const TilemapComponent &tilemap,
                                                const glm::vec3 &tilemapPosition,
                                                float &groundLevel,
                                                bool &onPlatform) -> bool
{
    glm::vec2 oldCentre = oldPosition + boxCollider.Offset;
    glm::vec2 newCentre = newPosition + boxCollider.Offset;

    const glm::vec2 bottomLeftOffset = boxCollider.Extents - glm::vec2(1, -1);

    glm::vec2 oldBottomLeft = glm::round(oldCentre - bottomLeftOffset);

    glm::vec2 newBottomLeft = glm::round(newCentre - bottomLeftOffset); 
    glm::vec2 newBottomRight = glm::round(glm::vec2(newBottomLeft.x + boxCollider.Extents.x * 2.0f - 2.0f, newBottomLeft.y));

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


            // Check if it is in bounds.
            if (index_x < 0 || index_x > TilemapData::TILEMAP_MAX_X_LENGTH - 1 || index_y < 0 || index_y > TilemapData::TILEMAP_MAX_Y_LENGTH - 1)
            {
                break;
            }

            auto& tileType = tilemap.MapData[index_y][index_x].Type;
            groundLevel = static_cast<float>(index_y) * tilemap.TileSize.y + tilemap.TileSize.y * 0.5f + tilemapPosition.y;
            if (tileType == Tile::TileType::Solid)
            {
                onPlatform = false;
                return true;
            }
            else if (tileType == Tile::TileType::OneWay && fabs(checkedTile.y - groundLevel) <= Physics::PlatformCollisionDetectionThreshold + oldPosition.y - newPosition.y)
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

auto PhysicsSystem::CheckTilemapCollisionCeiling(const glm::vec2 &oldPosition, const glm::vec2 &newPosition, const BoxCollider2DComponent &boxCollider, const TilemapComponent &tilemap, const glm::vec3 &tilemapPosition, float &ceilingLevel) -> bool
{
    glm::vec2 newCentre = newPosition + boxCollider.Offset;
    glm::vec2 oldCentre = oldPosition + boxCollider.Offset;

    ceilingLevel = 0.0f; 

    const glm::vec2 topRightOffset = boxCollider.Extents + glm::vec2(-1, 1);
    glm::vec2 oldTopRight = glm::round(oldCentre + topRightOffset);

    glm::vec2 newTopRight = glm::round(newCentre + topRightOffset);
    glm::vec2 newTopLeft = glm::round(glm::vec2(newTopRight.x - boxCollider.Extents.x * 2.0f + 2.0f, newTopRight.y));

    size_t destinationY = GetTileIndexYAtWorldPoint(tilemapPosition, tilemap.TileSize, newTopRight.y);
    size_t fromY = glm::min(GetTileIndexYAtWorldPoint(tilemapPosition, tilemap.TileSize, oldTopRight.y) + 1, destinationY);
    float inverseDistInTiles = 1.f / glm::max(static_cast<int>(glm::abs(destinationY - fromY)), 1);

    size_t index_x = 0;

    for (size_t index_y = fromY; index_y <= destinationY; ++index_y)
    {
        glm::vec2 topRight = Utility::Lerp(newTopRight, oldTopRight, static_cast<float>(glm::abs(destinationY - index_y)) * inverseDistInTiles);
        glm::vec2 topLeft = glm::vec2(topRight.x - boxCollider.Extents.x * 2.0f + 2.0f, topRight.y);
        // NOTE - Basically, scan from the bottom left to the bottom right of the AABB for tiles.
        for (glm::vec2 checkedTile = topLeft;; checkedTile.x += tilemap.TileSize.x)
        {
            checkedTile.x = glm::min(checkedTile.x, topRight.x);

            index_x = GetTileIndexXAtWorldPoint(tilemapPosition, tilemap.TileSize, checkedTile.x);

            // Check if it is in bounds.
            if (index_x < 0 || index_x > TilemapData::TILEMAP_MAX_X_LENGTH - 1 || index_y < 0 || index_y > TilemapData::TILEMAP_MAX_Y_LENGTH - 1)
            {
                break;
            }

            auto& tileType = tilemap.MapData[index_y][index_x].Type;
            if (tileType == Tile::TileType::Solid)
            {
                ceilingLevel = static_cast<float>(index_y) * tilemap.TileSize.y - tilemap.TileSize.y * 0.5f + tilemapPosition.y;
                return true;
            }

            if (checkedTile.x >= topRight.x)
            {
                break;
            }
        }
    }
    return false;
}

auto PhysicsSystem::CheckTilemapCollisionLeft(const glm::vec2 &oldPosition, const glm::vec2 &newPosition, const BoxCollider2DComponent &boxCollider, const TilemapComponent &tilemap, const glm::vec3 &tilemapPosition, float &wallX) -> bool
{
    glm::vec2 newCentre = newPosition + boxCollider.Offset;
    glm::vec2 oldCentre = oldPosition + boxCollider.Offset;

    wallX = 0.f;

    const glm::vec2 bottomLeftOffset = boxCollider.Extents + glm::vec2(1, 0);
    glm::vec2 oldBottomLeft = glm::round(oldCentre - bottomLeftOffset);

    glm::vec2 newBottomLeft = glm::round(newCentre - bottomLeftOffset);
    glm::vec2 newTopLeft = glm::round(newBottomLeft + glm::vec2(0, boxCollider.Extents.y * 2.0f));

    size_t index_y;

    size_t destinationX = GetTileIndexXAtWorldPoint(tilemapPosition, tilemap.TileSize, newBottomLeft.x);
    size_t fromX = glm::max(GetTileIndexXAtWorldPoint(tilemapPosition, tilemap.TileSize, oldBottomLeft.x) - 1, destinationX);
    float inverseDistInTiles = 1.f / glm::max(static_cast<int>(glm::abs(destinationX - fromX)), 1);
    
    for (size_t index_x = fromX; index_x >= destinationX; --index_x)
    {
        glm::vec2 bottomLeft = Utility::Lerp(newBottomLeft, oldBottomLeft, static_cast<float>(glm::abs(destinationX - index_x)) * inverseDistInTiles);
        glm::vec2 topLeft = bottomLeft + glm::vec2(0.f, boxCollider.Extents.y * 2.0f);

        for (glm::vec2 checkedTile = bottomLeft; ; checkedTile.y += tilemap.TileSize.y)
        {
            checkedTile.y = glm::min(checkedTile.y, topLeft.y);

            index_y = GetTileIndexYAtWorldPoint(tilemapPosition, tilemap.TileSize, checkedTile.y);

            // Check if it is in bounds.
            if (index_x < 0 || index_x > TilemapData::TILEMAP_MAX_X_LENGTH - 1 || index_y < 0 || index_y > TilemapData::TILEMAP_MAX_Y_LENGTH - 1)
            {
                break;
            }

            auto& tileType = tilemap.MapData[index_y][index_x].Type;
            if (tileType == Tile::TileType::Solid)
            {
                wallX = static_cast<float>(index_x) * tilemap.TileSize.x + tilemap.TileSize.x * 0.5f + tilemapPosition.x;
                return true;
            }
            
            if (checkedTile.y >= topLeft.y)
            {
                break;
            }
        }
    }

    return false;
}

auto PhysicsSystem::CheckTilemapCollisionRight(const glm::vec2 &oldPosition, const glm::vec2 &newPosition, const BoxCollider2DComponent &boxCollider, const TilemapComponent &tilemap, const glm::vec3 &tilemapPosition, float &wallX) -> bool
{
    glm::vec2 newCentre = newPosition + boxCollider.Offset;
    glm::vec2 oldCentre = oldPosition + boxCollider.Offset;

    wallX = 0.f;

    const glm::vec2 bottomRightOffset = glm::vec2(boxCollider.Extents.x, -boxCollider.Extents.y) + glm::vec2(1, 0);
    glm::vec2 oldBottomRight = glm::round(oldCentre + bottomRightOffset);
    glm::vec2 newBottomRight = glm::round(newCentre + bottomRightOffset);
    glm::vec2 newTopRight = glm::round(newBottomRight + glm::vec2(0, boxCollider.Extents.y * 2.0f));

    size_t index_y;

    size_t destinationX = GetTileIndexXAtWorldPoint(tilemapPosition, tilemap.TileSize, newBottomRight.x);
    size_t fromX = glm::min(GetTileIndexXAtWorldPoint(tilemapPosition, tilemap.TileSize, oldBottomRight.x) + 1, destinationX);
    float inverseDistInTiles = 1.f / glm::max(static_cast<int>(glm::abs(destinationX - fromX)), 1);
    
    for (size_t index_x = fromX; index_x <= destinationX; ++index_x)
    {
        glm::vec2 bottomRight = Utility::Lerp(newBottomRight, oldBottomRight, static_cast<float>(glm::abs(destinationX - index_x)) * inverseDistInTiles);
        glm::vec2 topRight = bottomRight + glm::vec2(0.f, boxCollider.Extents.y * 2.0f);

        for (glm::vec2 checkedTile = bottomRight;; checkedTile.y += tilemap.TileSize.y)
        {
            checkedTile.y = glm::min(checkedTile.y, topRight.y);

            index_y = GetTileIndexYAtWorldPoint(tilemapPosition, tilemap.TileSize, checkedTile.y);

            // Check if it is in bounds.
            if (index_x < 0 || index_x > TilemapData::TILEMAP_MAX_X_LENGTH - 1 || index_y < 0 || index_y > TilemapData::TILEMAP_MAX_Y_LENGTH - 1)
            {
                break;
            }

            auto& tileType = tilemap.MapData[index_y][index_x].Type;
            if (tileType == Tile::TileType::Solid)
            {
                wallX = static_cast<float>(index_x) * tilemap.TileSize.x - tilemap.TileSize.x * 0.5f + tilemapPosition.x;
                return true;
            }
            
            if (checkedTile.y >= topRight.y)
            {
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

auto PhysicsSystem::GetTileIndicesAtWorldPoint(const glm::vec3 &tilemapPosition, const glm::vec2 &tileSize, const glm::vec2& worldPosition) const -> Utility::Vector2ui
{
    return std::make_pair(static_cast<size_t>((worldPosition.x - tilemapPosition.x + tileSize.x * 0.5f) / tileSize.x),
        static_cast<size_t>((worldPosition.y - tilemapPosition.y + tileSize.y * 0.5f) / tileSize.y));
}

auto PhysicsSystem::GetTileWorldPosition(const glm::vec3& tilemapWorldPosition, const glm::vec2& tileSize, size_t index_x, size_t index_y) const -> glm::vec2
{
    return (glm::vec2(index_x, index_y) * tileSize) + glm::vec2(tilemapWorldPosition);
}

auto PhysicsSystem::GetTileWorldPosition(const glm::vec3& tilemapWorldPosition, const glm::vec2& tileSize, glm::vec2 column_row) const -> glm::vec2
{
    return (column_row * tileSize) + glm::vec2(tilemapWorldPosition);
}

auto PhysicsSystem::GetTileAtWorldPoint(const glm::vec3& tilemapWorldPosition, TilemapComponent &tilemap, const glm::vec2 &worldPosition) const -> Tile &
{
    return tilemap.MapData[GetTileIndexYAtWorldPoint(tilemapWorldPosition, tilemap.TileSize, worldPosition.y)][GetTileIndexXAtWorldPoint(tilemapWorldPosition, tilemap.TileSize, worldPosition.x)];
}

auto PhysicsSystem::AABBTest(const BoxCollider2DComponent &box1, const glm::vec2& box1pos, const BoxCollider2DComponent &box2, const glm::vec2& box2pos, glm::vec2 &outOverlap) -> bool
{
    if (box1.Extents.x == 0.f || box1.Extents.y == 0.f || box2.Extents.x == 0.f || box2.Extents.y == 0.f ||
        fabs((box1pos.x + box1.Offset.x) - (box2pos.x + box2.Offset.x)) > box1.Extents.x + box2.Extents.x ||
            fabs((box1pos.y + box1.Offset.y) - (box2pos.y + box2.Offset.y)) > box1.Extents.y + box2.Extents.y)
                return false;

    outOverlap = glm::vec2(
        glm::sign((box1pos.x + box1.Offset.x) - (box2pos.x + box2.Offset.x)) * 
            ((box1.Extents.x + box2.Extents.x)) - fabs((box1pos.x + box1.Offset.x) - (box2pos.x + box2.Offset.x)),
        glm::sign((box1pos.y + box1.Offset.y) - (box2pos.y + box2.Offset.y)) * 
            ((box1.Extents.y + box2.Extents.y)) - fabs((box1pos.y + box1.Offset.y) - (box2pos.y + box2.Offset.y))
    );

    return true;
}

auto PhysicsSystem::RemoveEntityFromQuadtree(Entity e) -> void
{
    auto& transform = coordinator->GetComponent<TransformComponent>(e);
    auto nearestTilemap = tilemapSystem->GetClosestTilemap(glm::vec2(transform.Position));

    auto& physics_quadtree_component = coordinator->GetComponent<PhysicsQuadtreeComponent>(e);
    auto& areas = physics_quadtree_component.Areas;
    auto& entities_in_areas = physics_quadtree_component.EntitiesInAreas;
    auto& tilemap = coordinator->GetComponent<TilemapComponent>(nearestTilemap);

    for (size_t i = 0; i < areas.size(); ++i)
    {
        RemoveObjectFromArea(areas[i], tilemap, entities_in_areas[i], e);

        for (auto& ent : entities_in_areas)
        {
            auto& pqc = coordinator->GetComponent<PhysicsQuadtreeComponent>(ent);
            for (size_t j = 0; j < pqc.Collisions.size(); ++j)
            {
                if (pqc.Collisions[j].OtherEntity == e)
                {
                    Utility::RemoveAt(pqc.Collisions, j);
                    break;
                }
            }
        }

        Utility::RemoveAt(areas, i);
        Utility::RemoveAt(entities_in_areas, i);
    }

    physics_quadtree_component.Collisions.clear();
}

auto PhysicsSystem::UpdateAreas(TilemapComponent& nearestTilemap, const glm::vec3& tilemapWorldPosition, Entity e, TransformComponent& transform, BoxCollider2DComponent& collider) -> void
{
    glm::vec2 pos_vec2 = glm::vec2(transform.Position);
    glm::vec2 offsetPosition = pos_vec2 + collider.Offset;

    auto topLeftTile = GetTileIndicesAtWorldPoint(tilemapWorldPosition, nearestTilemap.TileSize, offsetPosition + glm::vec2(-collider.Extents.x, collider.Extents.y)); 
    auto topRightTile = GetTileIndicesAtWorldPoint(tilemapWorldPosition, nearestTilemap.TileSize, offsetPosition + collider.Extents);
    auto bottomLeftTile = GetTileIndicesAtWorldPoint(tilemapWorldPosition, nearestTilemap.TileSize, offsetPosition - collider.Extents);
    auto bottomRightTile = std::make_pair<size_t, size_t>(0, 0);

    topLeftTile.first /= nearestTilemap.QuadtreeGridAreaWidth;
    topLeftTile.second /= nearestTilemap.QuadtreeGridAreaHeight;

    topRightTile.first /= nearestTilemap.QuadtreeGridAreaWidth;
    topRightTile.second /= nearestTilemap.QuadtreeGridAreaHeight;

    bottomLeftTile.first /= nearestTilemap.QuadtreeGridAreaWidth;
    bottomLeftTile.second /= nearestTilemap.QuadtreeGridAreaHeight;

    // NOTE - This is all on the assumption that the entity will never be outside of the tilemap. Hence, there should
    // NOTE - be preliminary checks of the tilemap.

    // No point calculating it when theres already calculated values.
    bottomRightTile.first = topRightTile.second;
    bottomRightTile.second = bottomLeftTile.second;

    std::array<Utility::Vector2ui, 4> overlappingAreas;
    size_t count = 0;

    if (topLeftTile.first == topRightTile.first && topLeftTile.second == bottomLeftTile.second)
    {
        overlappingAreas[0] = (topLeftTile);
        count = 1;
    }
    else if (topLeftTile.first == topRightTile.first)
    {
        overlappingAreas[0] = (topLeftTile);
        overlappingAreas[1] = (bottomLeftTile);
        count = 2;
    }
    else if (topLeftTile.second == bottomLeftTile.second)
    {
        overlappingAreas[0] = (topLeftTile);
        overlappingAreas[1] = (topRightTile);
        count = 2;
    }
    else
    {
        overlappingAreas[0] = (topLeftTile);
        overlappingAreas[1] = (bottomLeftTile);
        overlappingAreas[2] = (topRightTile);
        overlappingAreas[3] = (bottomRightTile);
        count = 4;
    }

    auto& physics_quadtree_component = coordinator->GetComponent<PhysicsQuadtreeComponent>(e);
    auto& areas = physics_quadtree_component.Areas;
    auto& entities_in_areas = physics_quadtree_component.EntitiesInAreas;

    for (size_t i = 0; i < areas.size(); ++i)
    {
        if (!Utility::Contains(overlappingAreas, areas[i]))
        {
            RemoveObjectFromArea(areas[i], nearestTilemap, entities_in_areas[i], e);

            Utility::RemoveAt(areas, i);
            Utility::RemoveAt(entities_in_areas, i);
            --i;
        }
    }

    for (size_t i = 0; i < count; ++i)
    {
        auto& area = overlappingAreas[i];
        if (!Utility::Contains(areas, area))
        {
            AddEntityToArea(area, nearestTilemap, e);
        }
    }
}

auto PhysicsSystem::AddEntityToArea(Utility::Vector2ui areaIndices, TilemapComponent& nearestTilemap, Entity e) -> void
{
    auto& area = nearestTilemap.ObjectsInArea[areaIndices.first][areaIndices.second];

    auto& quadtree_component = coordinator->GetComponent<PhysicsQuadtreeComponent>(e);
    quadtree_component.Areas.emplace_back(areaIndices);
    quadtree_component.EntitiesInAreas.emplace_back(area.size());

    area.emplace_back(e);
}

auto PhysicsSystem::RemoveObjectFromArea(Utility::Vector2ui areaIndices, TilemapComponent& nearestTilemap, size_t indexInArea, Entity e) -> void
{
    auto& area = nearestTilemap.ObjectsInArea[areaIndices.first][areaIndices.second];

    auto tempEntity = area.back();
    std::swap(area.back(), area[indexInArea]);

    auto& tempEntityQuadtreeComponent = coordinator->GetComponent<PhysicsQuadtreeComponent>(tempEntity);
    auto& tempIDs = tempEntityQuadtreeComponent.EntitiesInAreas;
    auto& tempAreas = tempEntityQuadtreeComponent.Areas;

    for (size_t i = 0; i < tempAreas.size(); ++i)
    {
        if (tempAreas[i] == areaIndices)
        {
            tempIDs[i] = indexInArea;
            break;
        }
    }

    area.pop_back();
}
