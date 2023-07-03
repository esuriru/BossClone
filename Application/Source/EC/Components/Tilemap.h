#pragma once

#include "EC/Component.h" 

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

class Tilemap : public Component
{
public:
    constexpr static uint32_t MaxHorizontalLength = 128;
    constexpr static uint32_t MaxVerticalLength = 72;

public:
    Tilemap(GameObject& gameObject);

    glm::vec2 tileSize;
    float tilingFactor;
    void LoadCSV(const std::string& textureCsvFilePath, const std::string& typeCsvFilePath);

private:
    std::array<Ref<SubTexture2D>, std::numeric_limits<uint8_t>::max()> subTextureMap_{};
    std::array<std::array<Tile, MaxHorizontalLength>, MaxVerticalLength> tileData_{};

};