#pragma once

#include "EC/Component.h"
#include "Tilemap.h"

#include <vector>

class MazeController : public Component
{
public:
    MazeController(GameObject& gameObject);

    void Generate(const glm::ivec2& startPoint);
    void Start() override;

    inline MazeController* SetTilemap(Ref<Tilemap> tilemap)
    {
        tilemap_ = tilemap;
        return this;
    }

    inline MazeController* SetWallTextureIndex(uint8_t index)
    {
        wallIndex_ = index;
        return this;
    }

    inline MazeController* SetEmptyTextureIndex(uint8_t index)
    {
        emptyIndex_ = index;
        return this;
    }
private:
    Ref<Tilemap> tilemap_;

    uint8_t wallIndex_ = 0, 
        emptyIndex_ = 0;

    void SetTileEmpty(Tile& tile);
    void SetTileWall(Tile& tile);
    std::vector<glm::ivec2> GetUnvisitedNeighbours(
        const glm::ivec2& point);
};