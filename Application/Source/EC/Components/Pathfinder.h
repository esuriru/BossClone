#pragma once

#include <functional>
#include <queue>
#include <vector>

#include "EC/GameObject.h"
#include "EC/Component.h"
#include "EC/Components/Tilemap.h"

#include "Utils/Heuristic.h"

using HeuristicFunction = std::function<unsigned int(const glm::ivec2&,
    const glm::ivec2&, int)>;

class Pathfinder : public Component
{
public:
    Pathfinder(GameObject& gameObject);

    std::vector<glm::ivec2> Pathfind(const glm::ivec2& start,
        const glm::ivec2& target, HeuristicFunction func = Heuristic::Euclidean,
        const int weight = 10);
    
    inline Ref<Tilemap> GetReferenceTilemap() const
    {
        return tilemap_;
    }

    void Start() override;

private:
    Ref<Tilemap> tilemap_;

    glm::ivec2 start_, target_;
    HeuristicFunction heuristic_;
    int weight_;

    std::priority_queue<Tile> open_;
    std::vector<bool> closed_;
    std::vector<Tile> cameFrom_;
    uint32_t directionsCount_;

    std::vector<glm::ivec2> BuildPath() const;
    bool IsValid(const glm::ivec2& pos) const;
    bool IsBlocked(const unsigned int row,
        const unsigned int column) const;
    
    void ResetData();
    int ConvertTo1D(const glm::ivec2& pos) const;
};