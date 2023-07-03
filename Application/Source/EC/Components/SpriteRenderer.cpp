#include "SpriteRenderer.h"
#include "EC/GameObject.h"
#include "Renderer/Renderer2D.h"

SpriteRenderer::SpriteRenderer(Ref<GameObject> gameObject)
    : Renderer(gameObject)
    , colour(1.0f)
    , tilingFactor(1.0f)
{
    transform_ = gameObject->GetTransform();
}

void SpriteRenderer::Render()
{
    Renderer2D::DrawQuad(transform_->GetWorldMatrix(), subtexture_, tilingFactor, colour);
}

void SpriteRenderer::SetTexture(Ref<SubTexture2D> subtexture)
{
    subtexture_ = subtexture;
}

void SpriteRenderer::SetTexture(Ref<Texture2D> texture)
{
    subtexture_ = CreateRef<SubTexture2D>(texture, glm::vec2(), glm::vec2(1.0f));
}