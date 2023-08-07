#include "BatSystem.h"
#include "Events/ApplicationEvent.h"
#include "Core/Log.h"
#include "ECS/Systems/PlayerSystem.h"
#include "ECS/Component.h"
#include "Game/Tilemap.h"
#include <utility>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include "Utils/Input.h"
#include "Core/KeyCodes.h"

static Coordinator* coordinator = Coordinator::Instance();

BatSystem::BatSystem()
    : currentState_(IDLE)
{
}

void BatSystem::Update(Timestep ts)
{
    if (entities.size() == 0) return;
    for (auto& e : entities)
    {
        if (!hasEnabledAnimation_)
        {
            hasEnabledAnimation_ = true;
            AnimationEvent event(Animation::AnimationType::Flying, e, true);
            eventCallback(event);
        }

        StateUpdate(e, ts);
    }
}

auto BatSystem::OnPlayerEnterEvent(PlayerEnterEvent &e) -> bool
{
    if (entities.find(e.GetTargetEntity()) == entities.end())
        return false;

    auto& player_health = coordinator->GetComponent<HealthComponent>(e.GetPlayerEntity());
    if (player_health.CurrentCooldownFrames > 0)
        return false;

    auto& bat = coordinator->GetComponent<BatComponent>(e.GetTargetEntity());

    player_health.Health -= bat.Damage;
    player_health.CurrentCooldownFrames += player_health.CooldownFramesOnHit;
    player_health.OnIFrameBehaviour(e.GetPlayerEntity());

    // TODO - Fix the health system such that the player can still detect pickups while not taking damage.
    // auto& physics_quadtree = coordinator->GetComponent<PhysicsQuadtreeComponent>(e.GetPlayerEntity());
    // physics_quadtree.Active = false;

    if (player_health.Health <= 0)
    {
        player_health.OnDeathBehaviour(e.GetPlayerEntity());
    }
    
    return false;
}

void BatSystem::StateUpdate(Entity e, Timestep ts)
{
    static float timer = 0.0f;
    static const float patrolTime = 1.0f;
    static const float idleTime = 1.0f;
    static glm::vec2 direction = glm::vec2(1, 0);
    
    Entity player = *(playerSystem->entities.begin());
    Entity tilemapEnt = coordinator->GetComponent<PhysicsQuadtreeComponent>(e).LastKnownTilemap;

    auto& bat = coordinator->GetComponent<BatComponent>(e);
    if (tilemapEnt == 0) return;
    auto& tilemap = coordinator->GetComponent<TilemapComponent>(tilemapEnt);
    auto& tilemapTransform = coordinator->GetComponent<TransformComponent>(tilemapEnt);
    auto& tilemapPos = tilemapTransform.Position; 
    auto& playerPos = coordinator->GetComponent<TransformComponent>(player).Position;
    auto& transform = coordinator->GetComponent<TransformComponent>(e);
    auto& batPos = transform.Position;
    auto& rigidbody = coordinator->GetComponent<RigidBody2DComponent>(e);
    static const float chaseRange = 8000.f;

    switch (currentState_)
    {
        case IDLE:
            rigidbody.LinearVelocity = glm::vec2(); 
            timer += ts; 
            if (glm::distance2(playerPos, batPos) < chaseRange)
            {
                currentState_ = CHASE;
            }
            else if (timer >= idleTime)
            {
                currentState_ = PATROL;
                timer = 0.0f;
            }
            break;
        case PATROL:
            rigidbody.LinearVelocity = direction * bat.FlightSpeed * 0.25f;
            timer += ts;
            if (glm::distance2(playerPos, batPos) < chaseRange)
            {
                currentState_ = CHASE;
            }
            else if (timer >= patrolTime)
            {
                direction.x = -direction.x;
                currentState_ = IDLE;
                timer = 0.0f;
            }
            break;
        case CHASE:
            {
                if (glm::distance2(playerPos, batPos) < chaseRange)
                {
                    std::pair<size_t, size_t> playerIndices = std::make_pair(static_cast<size_t>((playerPos.x - tilemapPos.x + tilemap.TileSize.x * 0.5f) / tilemap.TileSize.x),
                        static_cast<size_t>((playerPos.y - tilemapPos.y + tilemap.TileSize.y * 0.5f) / tilemap.TileSize.y));

                    // CC_TRACE("Player indices: ", playerIndices.first, ", ", playerIndices.second);
                    std::pair<size_t, size_t> batIndices = std::make_pair(static_cast<size_t>((batPos.x - tilemapPos.x + tilemap.TileSize.x * 0.5f) / tilemap.TileSize.x),
                        static_cast<size_t>((batPos.y - tilemapPos.y + tilemap.TileSize.y * 0.5f) / tilemap.TileSize.y));
                    // CC_TRACE("Bat indices: ", batIndices.first, ", ", batIndices.second);

                    auto path = tilemap.Pathfind(glm::vec2(batIndices.first, batIndices.second), glm::vec2(playerIndices.first, playerIndices.second), Heuristic::Euclidean, 10);

                    bool firstPosition = true;
                    glm::vec2 pathPoint;
                    
                    for (const auto& coord : path)
                    {
                        pathPoint = coord;
                        if (firstPosition)
                        {
                            bat.Destination = glm::vec2(pathPoint.x * tilemap.TileSize.x + tilemapPos.x, pathPoint.y * tilemap.TileSize.y + tilemapPos.y);
                            bat.Direction = glm::normalize(pathPoint - glm::vec2(batIndices.first, batIndices.second));
                            firstPosition = false;
                        }
                        else
                        {
                            if (glm::normalize(pathPoint - glm::vec2(batIndices.first, batIndices.second)) == bat.Direction)
                            {
                                bat.Destination = glm::vec2(pathPoint.x * tilemap.TileSize.x + tilemapPos.x, pathPoint.y * tilemap.TileSize.y + tilemapPos.y);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    if (path.size() > 0)
                    {
                        rigidbody.LinearVelocity = bat.Direction * bat.FlightSpeed;
                    }
                }
                else
                {
                    currentState_ = IDLE;
                }
            }
            break;
    };

    transform.Scale.x = rigidbody.LinearVelocity.x > 0 ? fabs(transform.Scale.x) : -fabs(transform.Scale.x);
}
