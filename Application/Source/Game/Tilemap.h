#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <functional>

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

    Tile() = default;
    Tile(const glm::vec2& pos, unsigned int f) : Position(pos), F(f) {}
    Tile(glm::vec2& pos, const glm::vec2& parent, 
        unsigned int f, unsigned int g, unsigned int h) 
    : Position(pos), Parent(parent), F(f), G(g), H(h) {}

    TileType Type = Empty;
    uint8_t TextureIndex;

    glm::vec2 Position; 
    glm::vec2 Parent;

    unsigned int F, G, H;

};

using HeuristicFunction = std::function<unsigned int(const glm::vec2&, const glm::vec2&, int)>;

inline bool operator< (const Tile& a, const Tile& b)
{
    return b.F < a.F;
}

namespace Heuristic
{
    unsigned int Manhattan(const glm::vec2& v1, const glm::vec2& v2, int weight);
    unsigned int Euclidean(const glm::vec2& v1, const glm::vec2& v2, int weight);
}

namespace TilemapData
{
    constexpr static std::array<glm::vec2, 8> DIRECTIONS
    {
        glm::vec2(-1, 0 ), glm::vec2(1, 0 ), glm::vec2( 0, 1 ), glm::vec2( 0, -1 ),
        glm::vec2(-1, -1 ), glm::vec2(1, 1 ), glm::vec2( -1, 1 ), glm::vec2( 1, -1 )
    };

    constexpr static uint32_t TILEMAP_MAX_X_LENGTH = 128;
    constexpr static uint32_t TILEMAP_MAX_Y_LENGTH = 72;
};

// NOTE - An instance of a TilemapComponent is like a chunk that will be shown to the screen.
// NOTE - There will probably be multiple lest the game be side-scrolling.
struct TilemapComponent
{
    // NOTE - The first element is left out purposely.
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

    // Quadtree 
    int QuadtreeGridAreaWidth = 64;  
    int QuadtreeGridAreaHeight = 64;  

    int HorizontalAreasCount = 0;
    int VerticalAreasCount = 0;
    unsigned int DirectionsCount = 4;
    
    std::string TilemapPath;
    std::string TilemapTypesPath;

    std::vector<std::vector<std::vector<Entity>>> ObjectsInArea;
    std::vector<glm::vec2> Pathfind(const glm::vec2& start, const glm::vec2& target, HeuristicFunction func, const int weight);

private:
    auto InitializeQuadtree() -> void;

    // A-star stuff


    std::vector<glm::vec2> BuildPath() const;
    bool IsValid(const glm::vec2& pos) const;
    bool IsBlocked(const unsigned int row, const unsigned int col, const bool invert = false) const;

    void ResetAStarLists();
    int ConvertTo1D(const glm::vec2& pos) const;

    glm::vec2 _start, _target;
    int _weight;
    HeuristicFunction _heuristic;

    std::priority_queue<Tile> _open;
    std::vector<bool> _closed;
    std::vector<Tile> _cameFrom;
    
};