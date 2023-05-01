#pragma once
#include "ECS/System.h"
#include "Core/Timestep.h"
#include "Core/Core.h"
#include <glm/glm.hpp>

#include "PhysicsComponent.h"
#include "ECS/Component.h"
#include "Game/Tilemap.h"
#include <utility>
#include <bitset>

namespace Physics
{
    constexpr float GravityAccelerationScalar = -9.8f;
    constexpr glm::vec2 GravityAccelerationVector = { 0.f, GravityAccelerationScalar };
    constexpr float TimeScale = 15.0f;
    constexpr float FrictionCoefficient = 0.98f;
    constexpr float PlatformCollisionDetectionThreshold = 2.0f;
}

class ActiveTilemapSystem : public System
{
public:
    auto Update(Timestep ts) -> void;
    auto GetClosestTilemap(const glm::vec2& position) -> Entity;
    auto GetTilemapCentre(TilemapComponent& tilemap, TransformComponent& transform) const -> glm::vec2;
    
};

class PhysicsSystem : public System
{
public:
    auto Update(Timestep ts) -> void;

    Entity tilemapEntity;
    Ref<ActiveTilemapSystem> tilemapSystem;

    static auto AddForce(RigidBody2DComponent& rigidbody, const glm::vec2& force, float deltaTime, Physics::ForceMode mode = Physics::ForceMode::Force) -> void;

    // Bitsets for determining at what state is the entity at. Using bitsets because entities are essentially IDs and it maximizes the memory efficiency.
    std::bitset<MaxEntities> onGroundBitset;
    std::bitset<MaxEntities> onPlatformBitset;
private:
    Timestep currentTimestep_;

    auto CheckTilemapCollision(const glm::vec2& oldPosition, const glm::vec2& newPosition,
        const BoxCollider2DComponent& boxCollider, const TilemapComponent& tilemap, const glm::vec3& tilemapPosition,
        float& groundLevel, bool& onPlatform) -> bool;

    auto AddForce(RigidBody2DComponent& rigidbody, const glm::vec2& force, Physics::ForceMode mode = Physics::ForceMode::Force) -> void;
    auto TransformPositionToTilemapLocal(const glm::vec2& worldPos) -> glm::vec2;

    auto GetTileIndexXAtWorldPoint(const glm::vec3& tilemapPosition, const glm::vec2& tileSize, const float x) const -> size_t;
    auto GetTileIndexYAtWorldPoint(const glm::vec3& tilemapPosition, const glm::vec2& tileSize, const float y) const -> size_t;

    auto GetTileWorldPosition(const glm::vec3& tilemapWorldPosition, const glm::vec2& tileSize, size_t index_x, size_t index_y) const -> glm::vec2;
    auto GetTileWorldPosition(const glm::vec3& tilemapWorldPosition, const glm::vec2& tileSize, glm::vec2 column_row) const -> glm::vec2;
};