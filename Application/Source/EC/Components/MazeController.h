#pragma once

#include "EC/Component.h"
#include "Tilemap.h"

#include <vector>

class MazeController : public Component
{
public:
    MazeController(GameObject& gameObject);

    glm::ivec2 Generate(const glm::ivec2& startPoint, int percentageWater = 5, int
        percentageBrokenWall = 10);
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

    inline MazeController* SetWaterTextureIndex(uint8_t index)
    {
        waterIndex_ = index;
        return this;
    }

    inline MazeController* SetBrokenWallTextureIndex(uint8_t index)
    {
        brokenWallIndex_ = index;
        return this;
    }

private:
    Ref<Tilemap> tilemap_;

    uint8_t wallIndex_ = 0, 
        emptyIndex_ = 0,
        waterIndex_ = 0,
        brokenWallIndex_ = 0;

    bool TestForExit(const glm::ivec2& location, glm::ivec2& exitLocation);

    void SetTileEmpty(Tile& tile);
    void SetTileWall(Tile& tile);
    void SetTileWater(Tile& tile);
    void SetTileBrokenWall(Tile& tile);

    std::vector<glm::ivec2> GetUnvisitedNeighbours(
        const glm::ivec2& point);
};