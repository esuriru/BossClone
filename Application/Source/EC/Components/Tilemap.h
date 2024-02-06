#pragma once

#include "EC/Component.h" 
#include "Core/Core.h"

#include <array>
#include <vector>
#include <limits>
#include <glm/glm.hpp>
#include <utility>
#include "Renderer/SubTexture2D.h"

struct Tile
{
    Tile() : weight(0), position({0, 0}), parent({-1, -1}), F(0), G(0), H(0) {}
    Tile(const glm::ivec2& pos, unsigned int f) : 
        weight(0), position(pos), parent(-1, 1), F(f) , G(0), H(0) {}
    Tile(glm::ivec2& pos, const glm::ivec2& parent, 
        unsigned int f, unsigned int g, unsigned int h) 
        : weight(0), position(pos), parent(parent), F(f), G(g), H(h) {}

    uint8_t textureIndex = 0;
    uint8_t weight = 0;

    bool weightAffectsPlayer = true;
    bool weightAffectsEnemies = true;
    float dot = 0.0f;

    glm::ivec2 position;
    glm::ivec2 parent;
    unsigned int F = 0, G = 0, H = 0;
};

inline bool operator<(const Tile& a, const Tile& b)
{
    return b.F < a.F;
}

class Tilemap : public Component, public std::enable_shared_from_this<Tilemap>
{
public:
    constexpr static uint32_t MaxHorizontalLength = 25;
    constexpr static uint32_t MaxVerticalLength = 25;
    constexpr static uint32_t MaxSize = MaxHorizontalLength * MaxVerticalLength;
    constexpr static std::array<glm::vec2, 8> Directions
    {
        glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(0, -1),
        glm::vec2(-1, -1), glm::vec2(1, 1), glm::vec2(-1, 1), glm::vec2(1, -1)
    };

public:
    Tilemap(GameObject& gameObject);
    Tilemap(GameObject& gameObject, const std::string& textureCsvFilePath, const std::string& typeCsvFilePath);
    
    glm::vec2 GetBounds();

    glm::vec4 colour;
    glm::vec2 tileSize;
    float tilingFactor;

    void ResetAllTiles(const int textureIndex = 0);
    void LoadCSV(const std::string& textureCsvFilePath, const std::string& typeCsvFilePath);

    Tile& GetTile(uint32_t x, uint32_t y); 
    Tile& GetTile(glm::ivec2 vec); 

    Ref<SubTexture2D>& GetTexture(uint32_t index);
    Ref<Tilemap> SetTexture(uint32_t index, Ref<SubTexture2D> subtexture);
    Ref<Tilemap> PushTexture(Ref<SubTexture2D> subtexture);

    glm::vec3 LocalToWorld(uint32_t x, uint32_t y);
    glm::vec3 LocalToWorld(std::pair<uint32_t, uint32_t> xyPair);
    glm::vec3 LocalToWorld(glm::ivec2 vec);

    static bool InBounds(glm::ivec2 tileCoordinates);
    static bool InBounds(int x, int y);

    void SetDataBounds(glm::ivec2 dataBoundsMin, glm::ivec2 dataBoundsMax);
    bool InDataBounds(glm::ivec2 coordinates);

    std::vector<std::reference_wrapper<Tile>> QueryTileWithWeight(int weight, 
        bool ignoreEdge = true);

    glm::ivec2 WorldToLocal(glm::vec3 worldPosition);

private:
    std::array<Ref<SubTexture2D>, std::numeric_limits<uint8_t>::max()> subTextureMap_{};
    std::array<std::array<Tile, MaxHorizontalLength>, MaxVerticalLength> tileData_{};
    glm::ivec2 dataBounds_;

    std::string textureCsvFilePath_, typeCsvFilePath_;
    uint32_t nextTextureIndex_;
    bool isPushed_;

};