#pragma once

#include <string>
#include <glad/glad.h>
#include "Core/Core.h"
#include <cstdint>
#include <glm/glm.hpp>

class Texture
{
public:
    virtual ~Texture() = default;
    virtual auto GetWidth() const -> uint32_t = 0;
    virtual auto GetHeight() const -> uint32_t = 0;
    
    virtual void SetData(void* data, uint32_t size) = 0;

    inline virtual auto GetSize() -> glm::vec2
    {
        return { GetWidth(), GetHeight() };
    }

    virtual auto Bind(uint32_t slot = 0) const -> void = 0;

    virtual auto operator==(const Texture& other) const -> bool = 0;
};

class Texture2D : public Texture
{
public:
    Texture2D(const std::string& path);
    Texture2D(uint32_t width, uint32_t height);
    ~Texture2D();

    auto SetData(void* data, uint32_t size) -> void;

    inline virtual auto GetWidth() const -> uint32_t override
    {
        return width_;
    }

    inline virtual auto GetHeight() const -> uint32_t override
    {
        return height_;
    }

    virtual auto Bind(uint32_t slot = 0) const -> void override;

    inline virtual auto operator==(const Texture& other) const -> bool override
    {
        // NOTE - for debugging only?
        return rendererID_ == dynamic_cast<const Texture2D&>(other).rendererID_;
    }

private:
    std::string path_;
    uint32_t rendererID_;

    uint32_t width_, height_;
    GLenum internalFormat_, dataFormat_;
};