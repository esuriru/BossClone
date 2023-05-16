#pragma once

#include <array>
#include "Texture.h"
#include "Core/Core.h"
#include <glm/glm.hpp>

class SubTexture2D
{
public:
    SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);
    SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max, uint32_t width, uint32_t height);

    inline auto GetTexture() const -> const Ref<Texture2D>
    {
        return texture_;
    }

    inline auto GetTexCoordsArray() const -> const std::array<glm::vec2, 4>&
    {
        return textureCoordinates_;
    }

    inline auto GetTexCoords() const -> const glm::vec2*
    {
        return textureCoordinates_.data();
    }

    inline auto GetWidth() const -> uint32_t 
    {
        return width_;
    }

    inline auto GetHeight() const -> uint32_t
    {
        return height_;
    }

    static auto CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1, 1 }) -> Ref<SubTexture2D>;

private:
    Ref<Texture2D> texture_;
    std::array<glm::vec2, 4> textureCoordinates_;
    uint32_t width_ = 0, height_ = 0;
};