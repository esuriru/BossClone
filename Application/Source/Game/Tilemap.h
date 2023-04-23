#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <string>

#include "Core/Core.h"
#include "Renderer/SubTexture2D.h"
#include "rapidcsv.h"

#include "ECS/Entity.h"

struct TilemapData
{
    constexpr static uint32_t TILEMAP_MAX_X_LENGTH = 128;
    constexpr static uint32_t TILEMAP_MAX_Y_LENGTH = 72;
};

// NOTE - An instance of a Tilemap is like a chunk that will be shown to the screen.
class Tilemap
{
public:
    Tilemap() : tilemapEntities_(TilemapData::TILEMAP_MAX_X_LENGTH * TilemapData::TILEMAP_MAX_Y_LENGTH), isFirstInit_(true) {}
    Tilemap(const std::string& csv_file_path);

    auto SetSubTexture(uint8_t tileID, const Ref<SubTexture2D>& texture, bool generateEntities = false) -> void;
    auto ImportTilemapCSV(const std::string& csv_file_path) -> void;

    auto GenerateEntities() -> void;

private:
    // The map of textures
    std::array<Ref<SubTexture2D>, std::numeric_limits<uint8_t>::max()> subTextureMap_ {};

    // The map represented in a 2D array.
    std::array<std::array<uint8_t, TilemapData::TILEMAP_MAX_X_LENGTH>, TilemapData::TILEMAP_MAX_Y_LENGTH> mapData_{};
    std::vector<Entity> tilemapEntities_ {};

    bool isFirstInit_;


};