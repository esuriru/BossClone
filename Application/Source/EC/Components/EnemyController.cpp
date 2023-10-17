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

}

void EnemyController::Update(Timestep ts)
{
    if (Input::Instance()->IsKeyPressed(Key::L))
    {
        auto localPosition = tilemap_->WorldToLocal(GetTransform().GetPosition());
        GetTransform().SetPosition(tilemap_->LocalToWorld(localPosition.first + 1, localPosition.second));
    }
}

void EnemyController::SetTilemap(Ref<Tilemap> tilemap)
{
    tilemap_ = tilemap;
}
