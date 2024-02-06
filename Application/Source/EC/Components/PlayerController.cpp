#include "PlayerController.h"

#include "Utils/Input.h"
#include "Core/KeyCodes.h"
#include "EC/GameObject.h"

#include <glm/gtx/string_cast.hpp>
#include "Game/GameManager.h"

PlayerController::PlayerController(GameObject &gameObject)
    : TilemapEntity(gameObject)
{
        
}

void PlayerController::Start()
{
    TilemapEntity::Start();
}

void PlayerController::Update(Timestep ts)
{
    static Input* input = Input::Instance();

    constexpr uint32_t directionCount = 4;

    constexpr std::array<decltype(Key::Up), directionCount> attackKeys 
    {
        {
            Key::I,
            Key::K,
            Key::J,
            Key::L
        }
    };

    constexpr std::array<decltype(Key::Up), directionCount> moveKeys 
    {
        {
            Key::Up,
            Key::Down,
            Key::Left,
            Key::Right
        }
    };

    constexpr std::array<glm::ivec2, directionCount> directions
    {
        {
            glm::ivec2(0, 1),
            glm::ivec2(0, -1),
            glm::ivec2(-1, 0),
            glm::ivec2(1, 0)
        }
    };

    if (!isCurrentTurn_)
    {
        return;
    }

    TilemapEntity::Update(ts);

    if (isMoving_)
    {
        return;
    }

    for (int i = 0; i < directionCount; ++i)
    {
        if (input->IsKeyPressed(moveKeys[i]))
        {
            auto newTilemapPosition = tilemapPosition_ + directions[i];
            auto& tile = tilemap_->GetTile(newTilemapPosition);
            if (!tilemap_->InBounds(newTilemapPosition) ||
                tile.weight <= 0)
            {
                continue;
            }

            QueueMove(tilemap_->LocalToWorld(newTilemapPosition),
                movementTime_,
                [=]()
                {
                    SetNearbyTilesVisible(tilemapPosition_, false);
                    tilemapPosition_ = newTilemapPosition; 
                    // SetNearbyTilesVisible(tilemapPosition_, true);

                    if (TestForWin(tilemapPosition_))
                    {
                        GameManager::Instance()->NewGame();
                    }
                    else
                    {
                        isCurrentTurn_ = false;
                        GameManager::Instance()->OnTurnFinish();
                    }
                    // CC_TRACE(glm::to_string(tilemapPosition_));
                },
                tile.weightAffectsPlayer ? tile.weight - 1 : 0);
            break;
        }
        else if (input->IsKeyPressed(attackKeys[i]))
        {
            auto newTilemapPosition = tilemapPosition_ + directions[i];
            auto& tile = tilemap_->GetTile(newTilemapPosition);
            if (!tilemap_->InBounds(newTilemapPosition) ||
                tile.weight <= 0)
            {
                continue;
            }

            std::vector<glm::ivec2> tilemapLocations
            {
                {
                    newTilemapPosition
                }
            };

            for (auto& entity : GameManager::Instance()->QueryTiles(tilemapLocations))
            {
                if (entity->GetGameObject().CompareTag(gameObject_.GetTag()))
                {
                    continue;
                }

                entity->TakeDamage(baseDamage_ + 
                    static_cast<float>(rand() % 11));
                isCurrentTurn_ = false;
                GameManager::Instance()->OnTurnFinish();
                break;
            }
        }
        else if (input->IsKeyPressed(Key::Space))
        {
            std::vector<glm::ivec2> tilemapLocations
            {
                {
                    tilemapPosition_
                }
            };

            for (auto& entity : GameManager::Instance()->QueryTiles(tilemapLocations))
            {
                if (entity->GetGameObject().CompareTag(gameObject_.GetTag()))
                {
                    continue;
                }

                entity->TakeDamage(baseDamage_ + 
                    static_cast<float>(rand() % 11));
                isCurrentTurn_ = false;
                GameManager::Instance()->OnTurnFinish();
                break;
            }
        }
    }
}

bool PlayerController::TestForWin(const glm::ivec2 &location)
{
    return (location.x == 0 || location.x == Tilemap::MaxHorizontalLength - 1 ||
        location.y == 0 || location.y == Tilemap::MaxVerticalLength - 1);
}

void PlayerController::OnDeath()
{
    GameManager::Instance()->NewGame();
}
