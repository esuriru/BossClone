#pragma once

#include "EC/Component.h" 
#include "Core/Core.h"

#include <array>
#include <limits>
#include <glm/glm.hpp>
#include "Renderer/SubTexture2D.h"

struct Tile
{
    enum 
    {
        Empty = 0,
        OneWay, 
        Solid
    } tileType = Empty;

    uint8_t textureIndex = 0;
};

class Tilemap : public Component, public std::enable_shared_from_this<Tilemap>
{
public:
    constexpr static uint32_t MaxHorizontalLength = 128;
    constexpr static uint32_t MaxVerticalLength = 72;

public:
    Tilemap(GameObject& gameObject);
    Tilemap(GameObject& gameObject, const std::string& textureCsvFilePath, const std::string& typeCsvFilePath);

    glm::vec4 colour;
    glm::vec2 tileSize;
    float tilingFactor;
    void LoadCSV(const std::string& textureCsvFilePath, const std::string& typeCsvFilePath);

    Tile& GetTile(uint32_t x, uint32_t y); 
    Ref<SubTexture2D>& GetTexture(uint32_t index);
    Ref<Tilemap> SetTexture(uint32_t index, Ref<SubTexture2D> subtexture);
    Ref<Tilemap> PushTexture(Ref<SubTexture2D> subtexture);

private:
    std::array<Ref<SubTexture2D>, std::numeric_limits<uint8_t>::max()> subTextureMap_{};
    std::array<std::array<Tile, MaxHorizontalLength>, MaxVerticalLength> tileData_{};

    std::string textureCsvFilePath_, typeCsvFilePath_;
    uint32_t nextTextureIndex_;
    bool isPushed_;

};