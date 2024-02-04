#include "TilemapEntity.h"

#include "Transform.h"
#include "Utils/Util.h"

TilemapEntity::TilemapEntity(GameObject &gameObject)
    : Component(gameObject)
    , isMoving_(false)
    , isCurrentTurn_(false)
    , moveCallback_(nullptr)
{

}

void TilemapEntity::Start()
{
    FixTilemapPosition();
    UpdateNearbyTilesVisibility();
}

void TilemapEntity::Update(Timestep ts)
{
    if (isMoving_)
    {
        timer_ += ts;
        GetTransform().SetPosition(Utility::Lerp(originalPosition_,
            targetPosition_, glm::min(timer_ * inverseMoveTime_, 1.0f)));
        if (timer_ >= moveTime_)
        {
            if (moveCallback_)
            {
                moveCallback_();
            }
            isMoving_ = false;
        }
    }
}

void TilemapEntity::StartTurn()
{
    isCurrentTurn_ = true;
}

void TilemapEntity::FixTilemapPosition()
{
    if (tilemap_)
    {
        tilemapPosition_ = 
            tilemap_->WorldToLocal(GetTransform().GetPosition());
        GetTransform().SetPosition(tilemap_->LocalToWorld(tilemapPosition_));
    }
}

void TilemapEntity::QueueMove(glm::vec3 targetPosition, float seconds,
    std::function<void()> callback)
{
    if (isMoving_)
    {
        return;
    }

    timer_ = 0.0f;
    originalPosition_ = GetTransform().GetPosition();
    targetPosition_ = targetPosition;
    inverseMoveTime_ = 1.0f / seconds;
    moveTime_ = seconds;
    isMoving_ = true;
    moveCallback_ = callback;
}

void TilemapEntity::UpdateNearbyTilesVisibility()
{
    SetNearbyTilesVisible(tilemapPosition_);
}

void TilemapEntity::SetNearbyTilesVisible(
    const glm::ivec2 &location, bool visible)
{
    auto tiles = GetNearbyTiles(location, visibilityRange_);
    for (auto& tile : tiles)
    {
        tile.get().textureIndex = static_cast<int>(!visible);
    }
}

std::vector<std::reference_wrapper<Tile>> TilemapEntity::GetNearbyTiles(
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
