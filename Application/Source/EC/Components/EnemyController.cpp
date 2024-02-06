#include "EnemyController.h"

#include "Utils/Input.h"

#include "Core/KeyCodes.h"
#include "Core/Core.h"

#include "EC/Components/Transform.h"
#include "EC/Components/Collider2D.h"
#include "EC/Components/BoxCollider2D.h"
#include "EC/GameObject.h"

#include "Game/StateMachine.h"
#include "Game/ActionEntry.h"

#include "Game/TimedTransition.h"

#include "Utils/Random.h"
#include "Utils/Util.h"

#include "Game/EightWayDirectionFlags.h"

#include "Tilemap.h"

#include "Game/EnemyPool.h"
#include "Game/GameManager.h"

#include <string>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/compatibility.hpp>

EnemyController::EnemyController(GameObject &gameObject)
    : TilemapEntity(gameObject)
    , stateMachine_(CreateScope<StateMachine<>>("Default", false))
{
    stateMachine_->AddState(CreateScope<State<>>(std::string("Patrol"),
        ActionEntry("Exit",
        [&]()
        {
            // NOTE - If access violation thrown, 
            // because enemy spawned instantly in chase state
        }),
        ActionEntry("Update",
        [&]()
        {
            if (isCurrentTurn_ && !isMoving_)
            {
                MoveWithDFS();
            }
        })
    ));

    stateMachine_->AddState(CreateScope<State<>>(std::string("Chase"),
        ActionEntry("Enter",
        [&]()
        {
            turnsInChase_ = 3;
        }),
        ActionEntry("Exit",
        [&]()
        {
            dfs_->Reset();
            dfs_->Init(tilemapPosition_);
        }),
        ActionEntry("Update",
        [&]()
        {
            if (isCurrentTurn_ && !isMoving_)
            {
                if (tilemapPosition_ == entityChasing_->GetTilemapPosition())
                {
                    Attack();
                    return;
                }

                auto path = pathfinder_->Pathfind(tilemapPosition_, 
                    entityChasing_->GetTilemapPosition());

                if (!path.empty())
                {
                    if (path.size() <= 1)
                    {
                        // Attack
                        Attack();
                    }
                    else
                    {
                        auto& tile = tilemap_->GetTile(path.front());
                        // Keep chasing
                        QueueMove(tilemap_->LocalToWorld(path.front()),
                            movementTime_,
                            [=]()
                            {
                                SetNearbyTilesVisible(tilemapPosition_, false);
                                tilemapPosition_ = path.front(); 

                                isCurrentTurn_ = false;
                                GameManager::Instance()->OnTurnFinish();
                            },
                            tile.weightAffectsEnemies ? tile.weight - 1 : 0);
                    }
                }
            }
        })
    ));

    stateMachine_->AddState(CreateScope<State<>>(std::string("Flee"),
        ActionEntry("Exit",
        [&]()
        {
            dfs_->Reset();
            dfs_->Init(tilemapPosition_);
        }),
        ActionEntry("Update",
        [&]()
        {
            if (isCurrentTurn_ && !isMoving_)
            {
                if (tilemapPosition_ == 
                    entityChasing_->GetTilemapPosition())
                {
                    MoveInRandomAvailableDirection();
                    return;
                }

                auto path = pathfinder_->Pathfind(tilemapPosition_, 
                    entityChasing_->GetTilemapPosition());
                
                if (!path.empty())
                {
                    glm::ivec2 delta = path.front() - tilemapPosition_;
                    auto directionToPlayer = Utility::DeltaToDirection(delta);
                    auto directions = 
                        Utility::GetOppositeAndAccessories(directionToPlayer);
                    
                    for (uint32_t i = 0; i < directions.size(); ++i)
                    {
                        auto newLocation = tilemapPosition_ + 
                            Utility::ConvertDirection(directions[i]);
                        
                        if (Tilemap::InBounds(newLocation))
                        {
                            auto& tile = tilemap_->GetTile(newLocation);
                            if (tile.weight <= 0)
                            {
                                continue;
                            }

                            // Run in any available direction, preferably opposite
                            QueueMove(tilemap_->LocalToWorld(newLocation),
                                movementTime_,
                                [=]()
                                {
                                    SetNearbyTilesVisible(tilemapPosition_, false);
                                    tilemapPosition_ = newLocation; 

                                    isCurrentTurn_ = false;
                                    GameManager::Instance()->OnTurnFinish();
                                },
                                tile.weightAffectsEnemies ? tile.weight - 1 : 0);
                            return;
                        }
                    }

                    auto& tile = tilemap_->GetTile(path.front());
                    QueueMove(tilemap_->LocalToWorld(path.front()),
                        movementTime_,
                        [=]()
                        {
                            SetNearbyTilesVisible(tilemapPosition_, false);
                            tilemapPosition_ = path.front(); 

                            isCurrentTurn_ = false;
                            GameManager::Instance()->OnTurnFinish();
                        },
                        tile.weightAffectsEnemies ? tile.weight - 1 : 0);
                }
            }
        })
    ));

    stateMachine_->AddTransition(CreateScope<Transition<>>(
        std::string("Patrol"),
        std::string("Chase"),
        [&]()
        {
            if (!isCurrentTurn_ /*|| currentHealth_ < initialHealth_ * 0.5f*/)
            {
                return false;
            }

            auto surroundingEntities = 
                GameManager::Instance()->QueryTiles(
                    GetNearbyTileLocations(tilemapPosition_, visibilityRange_));

            for (auto& entity : surroundingEntities)
            {
                if (entity->GetGameObject().CompareTag("Player"))
                {
                    entityChasing_ = entity;
                    return true;
                }
            }    

            return false;
        }
    ));

    // stateMachine_->AddTransition(CreateScope<Transition<>>(
    //     std::string("Patrol"),
    //     std::string("Flee"),
    //     [&]()
    //     {
    //         if (!isCurrentTurn_ || currentHealth_ >= initialHealth_ * 0.5f)
    //         {
    //             return false;
    //         }

    //         auto surroundingEntities = 
    //             GameManager::Instance()->QueryTiles(
    //                 GetNearbyTileLocations(tilemapPosition_, visibilityRange_));

    //         for (auto& entity : surroundingEntities)
    //         {
    //             if (entity->GetGameObject().CompareTag("Player"))
    //             {
    //                 entityChasing_ = entity;
    //                 return true;
    //             }
    //         }    

    //         return false;
    //     }
    // ));

    stateMachine_->AddTransition(CreateScope<Transition<>>(
        std::string("Chase"),
        std::string("Flee"),
        [&]()
        {
            if (!isCurrentTurn_ || isMoving_)
            {
                return false;
            }
            return currentHealth_ < initialHealth_ * 0.5f;
        }
    ));

    stateMachine_->AddTransition(CreateScope<Transition<>>(
        std::string("Chase"),
        std::string("Patrol"),
        [&]()
        {
            if (!isCurrentTurn_ || isMoving_)
            {
                return false;
            }

            auto surroundingEntities = 
                GameManager::Instance()->QueryTiles(
                    GetNearbyTileLocations(tilemapPosition_, visibilityRange_));

            if (!Utility::Contains(surroundingEntities, entityChasing_))
            {
                CC_TRACE("Turns minused. Current turns: ", turnsInChase_ - 1);
                if (--turnsInChase_ <= 0)
                {
                    entityChasing_ = nullptr;
                    CC_TRACE("Back to patrol");
                    return true;
                }
            }
            else
            {
                CC_TRACE("Back in position");
                // The player is in the surrounding entities, keep staying in
                // chase for at least 3 turns
                turnsInChase_ = 3;
            }

            return false;
        }
    ));

    stateMachine_->AddTransition(CreateScope<Transition<>>(
        std::string("Flee"),
        std::string("Patrol"),
        [&]()
        {
            if (!isCurrentTurn_ || isMoving_)
            {
                return false;
            }

            auto surroundingEntities = 
                GameManager::Instance()->QueryTiles(
                    GetNearbyTileLocations(tilemapPosition_, visibilityRange_));

            if (!Utility::Contains(surroundingEntities, entityChasing_))
            {
                entityChasing_ = nullptr;
                CC_TRACE("Back to patrol from fleeing");
                return true;
            }
            return false;
        }
    ));

    stateMachine_->InitiateStartState("Patrol");
}

void EnemyController::Start()
{
    TilemapEntity::Start();
    pathfinder_ = tilemap_->GetGameObject().GetComponent<Pathfinder>();
    dfs_->Init(tilemapPosition_);
}

void EnemyController::Update(Timestep ts)
{
    TilemapEntity::Update(ts);
    stateMachine_->Update(ts);
}

void EnemyController::FixedUpdate(float fixedDeltaTime)
{
    stateMachine_->FixedUpdate(fixedDeltaTime);
}

void EnemyController::Reset()
{
    timer_ = 0.0f;
    currentHealth_ = initialHealth_;
    stateMachine_->Reset();
}

void EnemyController::MoveWithDFS()
{
    // constexpr std::array<glm::ivec2, 4> Directions
    // {
    //     {
    //         { 0, 1 },
    //         { 1, 0 },
    //         { 0,-1 },
    //         { -1,0 },
    //     }
    // };

    // std::vector<glm::ivec2> possiblePositions;
    // for (int i = 0; i < 4; ++i)
    // {
    //     auto newPosition = tilemapPosition_ + Directions[i];
    //     if (tilemap_->InBounds(newPosition) &&
    //         tilemap_->GetTile(newPosition).weight >= 1)
    //     {
    //         possiblePositions.emplace_back(newPosition);     
    //     }
    // }

    auto newTilemapPosition = dfs_->Step();
    // auto newTilemapPosition = possiblePositions[
    //     rand() % possiblePositions.size()];
    auto& tile = tilemap_->GetTile(newTilemapPosition);
    QueueMove(tilemap_->LocalToWorld(newTilemapPosition),
        movementTime_,
        [=]()
        {
            SetNearbyTilesVisible(tilemapPosition_, false);
            tilemapPosition_ = newTilemapPosition; 

            isCurrentTurn_ = false;
            GameManager::Instance()->OnTurnFinish();
            // CC_TRACE(glm::to_string(tilemapPosition_));
        },
        tile.weightAffectsEnemies ? tile.weight - 1 : 0);
}

void EnemyController::MoveInRandomAvailableDirection()
{
    constexpr std::array<glm::ivec2, 4> Directions
    {
        {
            { 0, 1 },
            { 1, 0 },
            { 0,-1 },
            { -1,0 },
        }
    };

    std::vector<glm::ivec2> possiblePositions;
    for (int i = 0; i < 4; ++i)
    {
        auto newPosition = tilemapPosition_ + Directions[i];
        if (tilemap_->InBounds(newPosition) &&
            tilemap_->GetTile(newPosition).weight >= 1)
        {
            possiblePositions.emplace_back(newPosition);     
        }
    }

    auto newTilemapPosition = possiblePositions[
        rand() % possiblePositions.size()];
    auto& tile = tilemap_->GetTile(newTilemapPosition);
    QueueMove(tilemap_->LocalToWorld(newTilemapPosition),
        movementTime_,
        [=]()
        {
            SetNearbyTilesVisible(tilemapPosition_, false);
            tilemapPosition_ = newTilemapPosition; 

            isCurrentTurn_ = false;
            GameManager::Instance()->OnTurnFinish();
        },
        tile.weightAffectsEnemies ? tile.weight - 1 : 0);
}

void EnemyController::Attack()
{
    entityChasing_->TakeDamage(damage_ + static_cast<float>(rand() % 11));
    isCurrentTurn_ = false;
    GameManager::Instance()->OnTurnFinish();
}

void EnemyController::OnDeath()
{
    GameManager::Instance()->RemoveTilemapEntity(shared_from_this());
}
