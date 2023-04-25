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

#include <limits>

static Coordinator* coordinator = Coordinator::Instance();

auto ActiveTilemapSystem::Update(Timestep ts) -> void
{
}

auto PhysicsSystem::Update(Timestep ts) -> void
{
    if (entities.empty()) return;

    currentTimestep_ = ts;

    auto player_list = coordinator->View<PlayerController2DComponent>();
    CC_ASSERT(player_list.size() > 1, "There should be no more than one player.")

    bool playerExists = !player_list.empty();

    constexpr int8_t velocityIterations = 6;
    constexpr int8_t positionIterations = 2;

    constexpr float step = 1 / 60.f;
    static float accumulator = 0.f;

    accumulator += glm::min(static_cast<float>(ts), 0.25f);

    while (accumulator >= step) 
    {
        glm::vec3 proposedPosition; 
        for (auto& e : entities)
        {
            auto& transform = coordinator->GetComponent<TransformComponent>(e);
            auto& rigidbody = coordinator->GetComponent<RigidBody2DComponent>(e);

            AddForce(rigidbody, Physics::GravityAccelerationVector, Physics::ForceMode::Acceleration);
            proposedPosition = transform.Position + glm::vec3(rigidbody.LinearVelocity * static_cast<float>(step), 0.f);

            // Now check for collision.
            auto tilemaps = coordinator->View<TilemapComponent>();

            if (tilemaps.empty() || !playerExists)
                continue;
            
            // auto player = player_list.front(); 
            Entity nearestTilemapEntity;
            float distance = std::numeric_limits<float>::max();

            for (auto& tilemap : tilemaps)
            {
                glm::vec3 tilemapPosition = coordinator->GetComponent<TransformComponent>(tilemap).Position;
                // glm::vec3 playerPosition = coordinator->GetComponent<TransformComponent>(player).Position;
                glm::vec3 playerPosition = proposedPosition;

                const float distanceSquared = glm::distance2(tilemapPosition, playerPosition);
                if (distanceSquared < distance) 
                {
                    distance = distanceSquared;
                    nearestTilemapEntity = tilemap;
                }
            }

            tilemapEntity = nearestTilemapEntity;

            auto localTilemapPosition = TransformPositionToTilemapLocal(proposedPosition);
            auto& nearestTilemap = coordinator->GetComponent<TilemapComponent>(tilemapEntity);

            if (nearestTilemap.GetTileData(localTilemapPosition) != 0)
            {
                CC_TRACE("Collision detected.");
            }
            else
            {
                transform.Position = proposedPosition;
            }
        }

        accumulator -= step;
    }
}

auto PhysicsSystem::AddForce(RigidBody2DComponent& rigidbody, const glm::vec2& force, Physics::ForceMode mode) -> void
{
    if (rigidbody.BodyType == Physics::RigidBodyType::Kinematic) return;

    switch (mode)
    {
    case Physics::ForceMode::Acceleration:
        rigidbody.LinearVelocity += force * static_cast<float>(currentTimestep_);
        break;
    case Physics::ForceMode::Force:
        rigidbody.LinearVelocity += force * static_cast<float>(currentTimestep_) * rigidbody.GetInverseMass();
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
