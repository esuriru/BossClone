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
    std::array<std::array<uint8_t, TilemapData::TILEMAP_MAX_X_LENGTH>, TilemapData::TILEMAP_MAX_Y_LENGTH> MapData{};

    glm::vec4 Colour = glm::vec4(1.0f);
    glm::vec2 TileSize = glm::vec2(1.0f);
    float TilingFactor = 1.0f;
    bool Collidable = true;

    TilemapComponent() = default;
    TilemapComponent(const std::string& csv_file_path);

    auto ImportTilemapCSV(const std::string& csv_file_path) -> void;

};