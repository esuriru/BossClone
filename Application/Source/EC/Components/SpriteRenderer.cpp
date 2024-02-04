#include "SpriteRenderer.h"
#include "EC/GameObject.h"
#include "Renderer/Renderer2D.h"

SpriteRenderer::SpriteRenderer(GameObject& gameObject)
    : Renderer(gameObject)
    , colour(1.0f)
    , tilingFactor(1.0f)
{
    
}

SpriteRenderer::SpriteRenderer(GameObject& gameObject, Ref<SubTexture2D> subtexture)
    : Renderer(gameObject)
    , colour(1.0f)
    , tilingFactor(1.0f)
    , subtexture_(subtexture)
{
    
}

SpriteRenderer::SpriteRenderer(GameObject& gameObject, Ref<Texture2D> texture)
    : Renderer(gameObject)
    , colour(1.0f)
    , tilingFactor(1.0f)
    , subtexture_(CreateRef<SubTexture2D>(texture, glm::vec2(), glm::vec2(1.0f))
)
{
    
}

void SpriteRenderer::Render()
{
    if (subtexture_)
    {
        Renderer2D::DrawQuad(gameObject_.GetTransform().GetWorldMatrix(), subtexture_, tilingFactor, colour);
    }
    else
    {
        Renderer2D::DrawQuad(gameObject_.GetTransform().GetWorldMatrix(), colour);
    }
}

void SpriteRenderer::SetTexture(Ref<SubTexture2D> subtexture)
{
    subtexture_ = subtexture;
}

void SpriteRenderer::SetTexture(Ref<Texture2D> texture)
{
    subtexture_ = CreateRef<SubTexture2D>(texture, glm::vec2(), glm::vec2(1.0f), 
        texture->GetWidth(), texture->GetHeight());
}

void SpriteRenderer::SetNativeSize()
{
    gameObject_.GetTransform().SetScale(glm::vec3(subtexture_->GetWidth(), subtexture_->GetHeight(), 1.0f));
}
