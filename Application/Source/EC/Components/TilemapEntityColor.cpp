#include "TilemapEntityColor.h"

#include "Transform.h"
#include "EC/GameObject.h"
#include "SpriteRenderer.h"

TilemapEntityColor::TilemapEntityColor(GameObject &gameObject)
    : Component(gameObject)
{
}

void TilemapEntityColor::Update(Timestep ts)
{
    if (!transformToFollow_)
    {
        return;
    }

    GetTransform().SetPosition(transformToFollow_->GetPosition() + offset_);
}

void TilemapEntityColor::SetColor(const glm::vec4 &color)
{
    GetGameObject().GetComponent<SpriteRenderer>()->colour = color;
}
