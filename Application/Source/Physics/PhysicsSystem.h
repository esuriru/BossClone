#pragma once
#include "ECS/System.h"
#include "Core/Timestep.h"
#include "Core/Core.h"
#include <glm/glm.hpp>

#include "PhysicsComponent.h"
#include "Collision2D.h"
#include "ECS/Component.h"
#include "Game/Tilemap.h"
#include <utility>
#include <bitset>
#include "Utils/Util.h"

#include "Core/Window.h"

namespace Physics
{
    constexpr float GravityAccelerationScalar = -25.8f;
    constexpr glm::vec2 GravityAccelerationVector = { 0.f, GravityAccelerationScalar };
    constexpr float TimeScale = 15.0f;
    constexpr float FrictionCoefficient = 0.93f;
    constexpr float PlatformCollisionDetectionThreshold = 2.0f;
    constexpr float AirSpeedMultiplier = 0.3f;
}

class ActiveTilemapSystem : public System
{
public:
    auto Update(Timestep ts) -> void;
    auto GetClosestTilemap(const glm::vec2& position) -> Entity;
    auto GetTilemapCentre(TilemapComponent& tilemap, TransformComponent& transform) const -> glm::vec2;
    auto CheckCollisions() -> void;

    EventCallback eventCallback;

private:
    auto HasCollisionData(const PhysicsQuadtreeComponent& pqc, Entity e) -> bool;

};


class OnEntityDestroyedEvent;
class PhysicsSystem : public System
{
public:
    auto Update(Timestep ts) -> void;
    auto OnEvent(Event& e) -> void;

    // TODO - Find out why this exists
    Entity tilemapEntity;
    Ref<ActiveTilemapSystem> tilemapSystem;

    static auto AddForce(RigidBody2DComponent& rigidbody, const glm::vec2& force, float deltaTime, Physics::ForceMode mode = Physics::ForceMode::Force) -> void;

    // Bitsets for determining at what state is the entity at. Using bitsets because entities are essentially IDs and it maximizes the memory efficiency.
    std::bitset<MaxEntities> onGroundBitset;
    std::bitset<MaxEntities> onPlatformBitset;

    inline static auto AABBTest(const BoxCollider2DComponent& box1, const glm::vec3& box1pos, const BoxCollider2DComponent& box2, const glm::vec3& box2pos, glm::vec2& outOverlap) -> bool
    {
        return AABBTest(box1, glm::vec2(box1pos), box2, glm::vec2(box2pos), outOverlap);
    }

    static auto AABBTest(const BoxCollider2DComponent& box1, const glm::vec2& box1pos, const BoxCollider2DComponent& box2, const glm::vec2& box2pos, glm::vec2& outOverlap) -> bool;

    auto RemoveEntityFromQuadtree(Entity e) -> void;
    auto RemoveEntityFromSpecificQuadtree(Entity e, Entity tilemapEntity) -> void;

private:
    Timestep currentTimestep_;

    // TODO - stupid name.
    auto OnOnEntityDestroyedEvent(OnEntityDestroyedEvent& e) -> bool;

    // NOTE - There are additional overloads for tilemap collision when they transition over two different tilemaps,
    // NOTE - The continuous collision detection needs to have info of the other tilemap.

    auto CheckTilemapCollisionGround(const glm::vec2& oldPosition, const glm::vec2& newPosition,
        const BoxCollider2DComponent& boxCollider, const TilemapComponent& tilemap, const glm::vec3& tilemapPosition,
        const TilemapComponent& newTilemap, const glm::vec3& newTilemapPosition, float& groundLevel, bool& onPlatform) -> bool;

    auto CheckTilemapCollisionGround(const glm::vec2& oldPosition, const glm::vec2& newPosition,
        const BoxCollider2DComponent& boxCollider, const TilemapComponent& tilemap, const glm::vec3& tilemapPosition,
        float& groundLevel, bool& onPlatform) -> bool;

    auto CheckTilemapCollisionCeiling(const glm::vec2& oldPosition, const glm::vec2& newPosition,
        const BoxCollider2DComponent& boxCollider, const TilemapComponent& tilemap, const glm::vec3& tilemapPosition,
        float& ceilingLevel) -> bool;

    auto CheckTilemapCollisionCeiling(const glm::vec2& oldPosition, const glm::vec2& newPosition,
        const BoxCollider2DComponent& boxCollider, const TilemapComponent& tilemap, const glm::vec3& tilemapPosition,
        const TilemapComponent& newTilemap, const glm::vec3& newTilemapPosition, float& ceilingLevel) -> bool;

    auto CheckTilemapCollisionLeft(const glm::vec2& oldPosition, const glm::vec2& newPosition,
        const BoxCollider2DComponent& boxCollider, const TilemapComponent& tilemap, const glm::vec3& tilemapPosition,
        float& wallX) -> bool;

    auto CheckTilemapCollisionLeft(const glm::vec2& oldPosition, const glm::vec2& newPosition,
        const BoxCollider2DComponent& boxCollider, const TilemapComponent& tilemap, const glm::vec3& tilemapPosition,
        const TilemapComponent& newTilemap, const glm::vec3& newTilemapPosition, float& wallX) -> bool;

    auto CheckTilemapCollisionRight(const glm::vec2& oldPosition, const glm::vec2& newPosition,
        const BoxCollider2DComponent& boxCollider, const TilemapComponent& tilemap, const glm::vec3& tilemapPosition,
        float& wallX) -> bool;

    auto CheckTilemapCollisionRight(const glm::vec2& oldPosition, const glm::vec2& newPosition,
        const BoxCollider2DComponent& boxCollider, const TilemapComponent& tilemap, const glm::vec3& tilemapPosition,
        const TilemapComponent& newTilemap, const glm::vec3& newTilemapPosition, float& wallX) -> bool;
        
    auto AddForce(RigidBody2DComponent& rigidbody, const glm::vec2& force, Physics::ForceMode mode = Physics::ForceMode::Force) -> void;
    auto TransformPositionToTilemapLocal(const glm::vec2& worldPos) -> glm::vec2;

    auto GetTileIndexXAtWorldPoint(const glm::vec3& tilemapPosition, const glm::vec2& tileSize, const float x) const -> size_t;
    auto GetTileIndexYAtWorldPoint(const glm::vec3& tilemapPosition, const glm::vec2& tileSize, const float y) const -> size_t;
    
    auto GetTileIndicesAtWorldPoint(const glm::vec3& tilemapPosition, const glm::vec2& tileSize, const glm::vec2& worldPosition) const -> Utility::Vector2ui;

    auto GetTileWorldPosition(const glm::vec3& tilemapWorldPosition, const glm::vec2& tileSize, size_t index_x, size_t index_y) const -> glm::vec2;
    auto GetTileWorldPosition(const glm::vec3& tilemapWorldPosition, const glm::vec2& tileSize, glm::vec2 column_row) const -> glm::vec2;

    auto GetTileAtWorldPoint(const glm::vec3& tilemapWorldPosition, TilemapComponent& tilemap, const glm::vec2& worldPosition) const -> Tile&;


    // Quadtree
    auto UpdateAreas(TilemapComponent& nearestTilemap, const glm::vec3& tilemapWorldPosition, Entity e, TransformComponent& transform, BoxCollider2DComponent& collider) -> void;

    auto AddEntityToArea(Utility::Vector2ui areaIndices, TilemapComponent& nearestTilemap, Entity e) -> void;
    auto RemoveObjectFromArea(Utility::Vector2ui areaIndices, TilemapComponent& nearestTilemap, size_t indexInArea, Entity e) -> void;
};