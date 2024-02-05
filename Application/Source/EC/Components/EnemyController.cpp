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
        ActionEntry("Enter",
        [&]()
        {

        }),
        ActionEntry("Update",
        [&]()
        {
            if (isCurrentTurn_ && !isMoving_)
            {
                MoveInRandomAvailableDirection();
            }
        })
    ));

    stateMachine_->AddState(CreateScope<State<>>(std::string("Chase"),
        ActionEntry("Enter",
        [&]()
        {
        }),
        ActionEntry("Update",
        [&]()
        {
            if (isCurrentTurn_ && !isMoving_)
            {
                CC_TRACE("Move");
                auto path = pathfinder_->Pathfind(tilemapPosition_, 
                    entityChasing_->GetTilemapPosition());

                if (!path.empty())
                {
                    if (path.size() == 1)
                    {
                        // Attack

                    }
                    else
                    {
                        // Keep chasing
                        QueueMove(tilemap_->LocalToWorld(path.front()),
                            movementTime_,
                            [=]()
                            {
                                SetNearbyTilesVisible(tilemapPosition_, false);
                                tilemapPosition_ = path.front(); 

                                isCurrentTurn_ = false;
                                GameManager::Instance()->OnTurnFinish();
                            });
                    }
                }
            }
        })
    ));

    stateMachine_->AddTransition(CreateScope<Transition<>>(
        std::string("Patrol"),
        std::string("Chase"),
        [&]()
        {
            if (!isCurrentTurn_)
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

    stateMachine_->AddTransition(CreateScope<Transition<>>(
        std::string("Chase"),
        std::string("Patrol"),
        [&]()
        {
            if (!isCurrentTurn_)
            {
                return false;
            }

            auto surroundingEntities = 
                GameManager::Instance()->QueryTiles(
                    GetNearbyTileLocations(tilemapPosition_, visibilityRange_));

            if (!Utility::Contains(surroundingEntities, entityChasing_))
            {
                entityChasing_ = nullptr;
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
    QueueMove(tilemap_->LocalToWorld(newTilemapPosition),
        movementTime_,
        [=]()
        {
            SetNearbyTilesVisible(tilemapPosition_, false);
            tilemapPosition_ = newTilemapPosition; 

            isCurrentTurn_ = false;
            GameManager::Instance()->OnTurnFinish();
            // CC_TRACE(glm::to_string(tilemapPosition_));
        });
}
