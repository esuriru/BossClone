#include "PlayerController.h"

#include "Utils/Input.h"
#include "Core/KeyCodes.h"

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

    constexpr std::array<decltype(Key::Up), directionCount> keys 
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
        if (input->IsKeyPressed(keys[i]))
        {
            auto newTilemapPosition = tilemapPosition_ + directions[i];
            auto& tile = tilemap_->GetTile(newTilemapPosition);
            if (!tilemap_->InBounds(newTilemapPosition) ||
                tile.weight <= 0)
            {
                continue;
            }
            CC_TRACE(tile.weight);

            QueueMove(tilemap_->LocalToWorld(newTilemapPosition),
                movementTime_,
                [=]()
                {
                    SetNearbyTilesVisible(tilemapPosition_, false);
                    tilemapPosition_ = newTilemapPosition; 
                    // SetNearbyTilesVisible(tilemapPosition_, true);

                    isCurrentTurn_ = false;
                    GameManager::Instance()->OnTurnFinish();
                    // CC_TRACE(glm::to_string(tilemapPosition_));
                });
            break;
        }
    }
}
