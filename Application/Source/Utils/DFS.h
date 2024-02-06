#pragma once

#include <glm/glm.hpp>
#include <bitset>
#include <stack>
#include <array>

#include "EC/Components/Tilemap.h"

class DFS
{
public:
    enum class Order 
    {
        UDLR,
        LDRU,
        Random
    };

    DFS(Ref<Tilemap> tilemap);

    void Init(const glm::ivec2& currentLocation);
    glm::ivec2 Step();
    void Reset();
    void Flush();

private:
    std::bitset<Tilemap::MaxHorizontalLength * Tilemap::MaxVerticalLength> 
        visitedBitset_;
    Order order_;

    std::stack<glm::ivec2> steps_;
    Ref<Tilemap> tilemap_;
    glm::ivec2 originalTilemapLocation_;

    uint32_t ConvertTo1D(const glm::ivec2& currentLocation);
    std::vector<glm::ivec2> GetNeighbours(const glm::ivec2& location,
        const std::array<glm::ivec2, 4>* directions) const;
};