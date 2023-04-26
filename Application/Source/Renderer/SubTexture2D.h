#pragma once

#include <array>
#include "Texture.h"
#include "Core/Core.h"
#include <glm/glm.hpp>

class SubTexture2D
{
public:
    SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

    inline auto GetTexture() const -> const Ref<Texture2D>
    {
        return texture_;
    }

    inline auto GetTexCoords() const -> const glm::vec2*
    {
        return textureCoordinates_.data();
    }

    static auto CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1, 1 }) -> Ref<SubTexture2D>;

private:
    Ref<Texture2D> texture_;
    
    std::array<glm::vec2, 4> textureCoordinates_;
};