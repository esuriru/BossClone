#include "TilemapEntity.h"

#include "Transform.h"
#include "Utils/Util.h"

#include "Game/GameManager.h"

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
    // UpdateNearbyTilesVisibility();
    currentHealth_ = initialHealth_;
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
    if (tilemapPosition_ != glm::ivec2())
    {
        float dot = tilemap_->GetTile(tilemapPosition_).dot;
        if (dot > 0.0f)
        {
            TakeDamage(dot);
        }
    }

    if (turnsLocked_ <= 0)
    {
        isCurrentTurn_ = true;
    }
    else
    {
        turnsLocked_--;
        GameManager::Instance()->OnTurnFinish();
    }
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
    std::function<void()> callback, int turnsToLock)
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
    turnsLocked_ = turnsToLock;
}

void TilemapEntity::UpdateNearbyTilesVisibility()
{
    SetNearbyTilesVisible(tilemapPosition_);
}

void TilemapEntity::TakeDamage(float amount)
{
    currentHealth_ -= amount;
    if (currentHealth_ <= 0)
    {
        OnDeath();
    }
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

std::vector<glm::ivec2> TilemapEntity::GetNearbyTileLocations(
    const glm::ivec2 &location, uint8_t range)
{
    std::vector<glm::ivec2> tiles;

    for (int i = -range; i <= range; ++i)
    {
        for (int j = -range + glm::abs(i); 
            j <= range - glm::abs(i); ++j)
        {
            auto newPosition = location + glm::ivec2(i, j);

            if (visibilityTilemap_->InBounds(newPosition))
            {
                tiles.emplace_back(newPosition);
            }
        }
    }

    return tiles;
}
