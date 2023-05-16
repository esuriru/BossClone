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
    , width_(static_cast<uint32_t>(fabs(min.x - max.x)))
    , height_(static_cast<uint32_t>(fabs(min.y - max.y)))
{

}

SubTexture2D::SubTexture2D(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max, uint32_t width, uint32_t height)
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
    , width_(width)
    , height_(height)
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

    return CreateRef<SubTexture2D>(texture, min, max, static_cast<uint32_t>(cellSize.x * spriteSize.x), static_cast<uint32_t>(cellSize.y * spriteSize.y));
}
