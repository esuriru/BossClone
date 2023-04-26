#include "SubTexture2D.h"
#include <glm/glm.hpp>

SubTexture2D::SubTexture2D(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max)
    : texture_(texture)
    , textureCoordinates_
    {
        {
            { min.x, min.y },
            { max.x, min.y },
            { max.x, max.y },
            { min.x, max.y }
        }
    }
{
}

auto SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2 &coords, const glm::vec2 &cellSize, const glm::vec2& spriteSize) -> Ref<SubTexture2D>
{
    const glm::vec2& min = {
        (coords.x * cellSize.x) / texture->GetWidth(),
        (coords.y * cellSize.y) / texture->GetHeight()
    };

    const glm::vec2& max = {
        ((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(),
        ((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight()
    };

    return CreateRef<SubTexture2D>(texture, min, max);
}
