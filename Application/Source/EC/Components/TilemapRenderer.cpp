#include "TilemapRenderer.h"
#include "Transform.h" 
#include "Renderer/Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

TilemapRenderer::TilemapRenderer(GameObject& gameObject)
    : Renderer(gameObject)
{
}

void TilemapRenderer::Render()
{
    if (!tilemapComponent_) return;
    for (int i = 0; i < Tilemap::MaxVerticalLength; ++i)
    {
        for (int j = 0; j < Tilemap::MaxHorizontalLength; ++j)
        {
            if (tilemapComponent_->GetTile(i, j).textureIndex == 0)
                continue;

            glm::mat4 model = GetTransform().GetWorldMatrix() * glm::translate(glm::mat4(1.0f), 
                glm::vec3(
                    j * tilemapComponent_->tileSize.x,
                    i * tilemapComponent_->tileSize.y,
                0)) * glm::scale(glm::mat4(1.0f), glm::vec3(tilemapComponent_->tileSize, 0));

            Renderer2D::DrawQuad(
                model,
                tilemapComponent_->GetTexture(tilemapComponent_->GetTile(i, j).textureIndex),
                tilemapComponent_->tilingFactor,
                tilemapComponent_->colour);
        }
    }
     
}
