#include "EnemyController.h"

#include "Utils/Input.h"
#include "Core/KeyCodes.h"
#include "EC/Components/Transform.h"

EnemyController::EnemyController(GameObject &gameObject)
    : Component(gameObject)
{

}

void EnemyController::Start()
{
    localTilemapPosition_ = tilemap_->WorldToLocal(GetTransform().GetPosition());
}

void EnemyController::Update(Timestep ts)
{
    if (Input::Instance()->IsKeyPressed(Key::L))
    {
        ++localTilemapPosition_.first;
        GetTransform().SetPosition(tilemap_->LocalToWorld(localTilemapPosition_));
    }
}

void EnemyController::SetTilemap(Ref<Tilemap> tilemap)
{
    tilemap_ = tilemap;
}
