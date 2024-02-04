#include "MazeController.h"
#include "EC/GameObject.h"

#include <glm/glm.hpp>

#include <stack>
#include <vector>

MazeController::MazeController(GameObject &gameObject)
    : Component(gameObject)
{
    srand(static_cast<unsigned>(time(nullptr)));
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

void MazeController::Generate(const glm::ivec2& startPoint)
{
    tilemap_->ResetAllTiles(wallIndex_);
    // Generate a maze using DFS
    std::stack<glm::ivec2> stack;
    stack.push(startPoint);
    SetTileEmpty(tilemap_->GetTile(startPoint));

    while (!stack.empty())
    {
        glm::ivec2 currentPoint = stack.top();
        stack.pop();
        CC_TRACE(stack.size());

        auto neighbours = GetUnvisitedNeighbours(currentPoint);
        if (!neighbours.empty())
        {
            stack.push(currentPoint);

            int randomIndex = rand() % neighbours.size();
            glm::ivec2 chosenNeighbour = neighbours[randomIndex];

            SetTileEmpty(tilemap_->GetTile(chosenNeighbour));

            glm::ivec2 middlePoint = glm::ivec2(
                (currentPoint.x + chosenNeighbour.x) / 2,
                (currentPoint.y + chosenNeighbour.y) / 2);
            SetTileEmpty(tilemap_->GetTile(middlePoint));

            stack.push(chosenNeighbour);
        }
    }
}

void MazeController::Start()
{
    tilemap_ = GetGameObject().GetComponent<Tilemap>();
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
