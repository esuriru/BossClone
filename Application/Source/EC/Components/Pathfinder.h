#pragma once

#include <functional>
#include <queue>
#include <vector>

#include "EC/GameObject.h"
#include "EC/Component.h"
#include "EC/Components/Tilemap.h"

using HeuristicFunction = std::function<unsigned int(const glm::vec2&,
    const glm::vec2&, int)>;

class Pathfinder : public Component
{
private:
    Ref<Tilemap> tilemap_;

    glm::vec2 start_, target_;
    HeuristicFunction heuristic_;
    int weight_;

    std::priority_queue<Tile> open_;
    std::vector<bool> closed_;
    std::vector<Tile> cameFrom_;
    uint32_t directionsCount_;

    std::vector<glm::vec2> BuildPath() const;
    bool IsValid(const glm::vec2& pos) const;
    bool IsBlocked(const unsigned int row,
        const unsigned int column) const;
    
    void ResetData();
    int ConvertTo1D(const glm::vec2& pos) const;

public:
    Pathfinder(GameObject& gameObject);

    std::vector<glm::vec2> Pathfind(const glm::vec2& start,
        const glm::vec2& target, HeuristicFunction func,
        const int weight);
    
    inline Ref<Tilemap> GetReferenceTilemap() const
    {
        return tilemap_;
    }

    void Start() override;
};