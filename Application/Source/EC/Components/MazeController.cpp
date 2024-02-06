#include "MazeController.h"
#include "EC/GameObject.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <stack>
#include <vector>

MazeController::MazeController(GameObject &gameObject)
    : Component(gameObject)
{
}

std::vector<glm::ivec2> MazeController::GetUnvisitedNeighbours(
    const glm::ivec2& point)
{
    std::vector<glm::ivec2> neighbours;
    const std::array<glm::ivec2, 4> offsets 
    {
        {
            { -2, 0 },
            {  2, 0 },
            { 0, -2 },
            { 0,  2 },
        }
    };

    for (int i = 0; i < 4; ++i)
    {
        glm::ivec2 newPosition = point + offsets[i];

        if (tilemap_->InBounds(newPosition) &&
            tilemap_->GetTile(newPosition).weight == 0)
        {
            neighbours.emplace_back(newPosition);
        }
    }

    return neighbours;
}

glm::ivec2 MazeController::Generate(const glm::ivec2& startPoint, int percentageWater,
    int percentageBrokenWall)
{
    tilemap_->ResetAllTiles(wallIndex_);
    // Generate a maze using DFS
    std::stack<glm::ivec2> stack;
    stack.push(startPoint);
    SetTileEmpty(tilemap_->GetTile(startPoint));

    std::vector<glm::ivec2> possibleExitLocations{};
    glm::ivec2 exitLocation{}; 
    bool exitFound = false;

    while (!stack.empty())
    {
        glm::ivec2 currentPoint = stack.top();
        stack.pop();

        auto neighbours = GetUnvisitedNeighbours(currentPoint);
        if (!neighbours.empty())
        {
            stack.push(currentPoint);

            int randomIndex = rand() % neighbours.size();
            glm::ivec2 chosenNeighbour = neighbours[randomIndex];

            int percentage = rand() % 100;
            if (percentage >= percentageWater)
            {
                SetTileEmpty(tilemap_->GetTile(chosenNeighbour));
            }
            else
            {
                SetTileWater(tilemap_->GetTile(chosenNeighbour));
            }
            
            if (TestForExit(chosenNeighbour, exitLocation))
            {
                possibleExitLocations.emplace_back(exitLocation);
            }

            glm::ivec2 middlePoint = glm::ivec2(
                (currentPoint.x + chosenNeighbour.x) / 2,
                (currentPoint.y + chosenNeighbour.y) / 2);
            SetTileEmpty(tilemap_->GetTile(middlePoint));

            stack.push(chosenNeighbour);
        }
    }

    exitLocation = possibleExitLocations[rand() % possibleExitLocations.size()];
    // CC_ASSERT(exitLocation == glm::ivec2{}, "test");
    SetTileEmpty(tilemap_->GetTile(exitLocation));

    for (auto& tile : tilemap_->QueryTileWithWeight(0))
    {
        int percentage = rand() % 100;
        if (percentage < percentageBrokenWall)
        {
            SetTileBrokenWall(tile);
        }
    }

    return exitLocation;
    // CC_TRACE(glm::to_string(exitLocation));
}

void MazeController::Start()
{
    if (!tilemap_)
    {
        tilemap_ = GetGameObject().GetComponent<Tilemap>();
    }
}

bool MazeController::TestForExit(const glm::ivec2 &location, glm::ivec2& exitLocation)
{
    if (location.x == 1)
    {
        exitLocation = glm::ivec2(0, location.y);
        return true;
    }
    else if (location.x == Tilemap::MaxHorizontalLength - 2)
    {
        exitLocation = glm::ivec2(Tilemap::MaxHorizontalLength - 1, location.y);
        return true;
    }
    else if (location.y == 1)
    {
        exitLocation = glm::ivec2(location.x, 0);
        return true;
    }
    else if (location.x == Tilemap::MaxVerticalLength - 2)
    {
        exitLocation = glm::ivec2(location.x, Tilemap::MaxVerticalLength - 1);
        return true;
    }
    return false;
}

void MazeController::SetTileEmpty(Tile &tile)
{
    tile.weight = 1;
    tile.textureIndex = emptyIndex_;
}

void MazeController::SetTileWall(Tile &tile)
{
    tile.weight = 0;
    tile.textureIndex = wallIndex_;
}

void MazeController::SetTileWater(Tile &tile)
{
    tile.weight = 2;
    tile.textureIndex = waterIndex_;
    tile.weightAffectsEnemies = false;
}

void MazeController::SetTileBrokenWall(Tile &tile)
{
    tile.weight = 2;
    tile.textureIndex = brokenWallIndex_;
    tile.weightAffectsPlayer = false;
}
