#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <string>

#include "Core/Core.h"
#include "Renderer/SubTexture2D.h"
#include "rapidcsv.h"

#include "ECS/Entity.h"
#include "glm/glm.hpp"

#include <glm/gtx/string_cast.hpp>

struct Tile
{
    enum TileType
    {
        OneWay,
        Solid,
        Empty, 
    };

    TileType Type = Empty;
    uint8_t TextureIndex;
};

namespace TilemapData
{
    constexpr static uint32_t TILEMAP_MAX_X_LENGTH = 128;
    constexpr static uint32_t TILEMAP_MAX_Y_LENGTH = 72;
};

// NOTE - An instance of a TilemapComponent is like a chunk that will be shown to the screen.
struct TilemapComponent
{
    // The map of textures
    std::array<Ref<SubTexture2D>, std::numeric_limits<uint8_t>::max()> SubTextureMap {};

    // The map represented in a 2D array.
    std::array<std::array<Tile, TilemapData::TILEMAP_MAX_X_LENGTH>, TilemapData::TILEMAP_MAX_Y_LENGTH> MapData{};

    glm::vec4 Colour = glm::vec4(1.0f);
    glm::vec2 TileSize = glm::vec2(16.0f);
    float TilingFactor = 1.0f;

    TilemapComponent() = default;
    explicit TilemapComponent(const std::string& map_csv, bool make_tiles_solid);
    TilemapComponent(const std::string& map_csv, const char* tile_type_csv);

    // Import tilemap without having a CSV for tile types.
    auto ImportTilemapCSV(const std::string& map_csv, bool make_tiles_solid = true) -> void;

    // Import tilemap with both map and tile types CSVs 
    auto ImportTilemapCSV(const std::string& map_csv, const std::string& tile_type_csv) -> void;
};