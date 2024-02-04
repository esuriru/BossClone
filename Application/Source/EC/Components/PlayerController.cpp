#include "PlayerController.h"

#include "Utils/Input.h"
#include "Core/KeyCodes.h"

#include <glm/gtx/string_cast.hpp>

PlayerController::PlayerController(GameObject &gameObject)
    : TilemapEntity(gameObject)
{
        
}

void PlayerController::Start()
{
    TilemapEntity::Start();
    SetNearbyTilesVisible(tilemapPosition_);
}

void PlayerController::Update(Timestep ts)
{
    TilemapEntity::Update(ts);
    
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
                    SetNearbyTilesVisible(tilemapPosition_, true);
                    // CC_TRACE(glm::to_string(tilemapPosition_));
                });
            break;
        }
    }
}

void PlayerController::SetNearbyTilesVisible(
    const glm::ivec2 &location, bool visible)
{
    auto tiles = GetNearbyTiles(location, visibilityRange_);
    for (auto& tile : tiles)
    {
        tile.get().textureIndex = static_cast<int>(!visible);
    }
}

std::vector<std::reference_wrapper<Tile>> PlayerController::GetNearbyTiles(
    const glm::ivec2 &location, uint8_t range)
{
    std::vector<std::reference_wrapper<Tile>> tiles;

    for (int i = -range; i <= range; ++i)
    {
        for (int j = -range + glm::abs(i); 
            j <= range - glm::abs(i); ++j)
        {
            auto newPosition = location + glm::ivec2(i, j);

            if (visibilityTilemap_->InBounds(newPosition))
            {
                tiles.emplace_back(
                    visibilityTilemap_->GetTile(newPosition));
            }
        }
    }

    return tiles;
}
