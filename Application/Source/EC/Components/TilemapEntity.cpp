#include "TilemapEntity.h"

#include "Transform.h"
#include "Utils/Util.h"

TilemapEntity::TilemapEntity(GameObject &gameObject)
    : Component(gameObject)
    , isMoving_(false)
    , moveCallback_(nullptr)
{

}

void TilemapEntity::Start()
{
    FixTilemapPosition();
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
