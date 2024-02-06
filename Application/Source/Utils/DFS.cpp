#include "DFS.h"

#include <array>

DFS::DFS(Ref<Tilemap> tilemap)
    : tilemap_(tilemap)
    , order_(DFS::Order::UDLR)
{
    Reset();
}

void DFS::Init(const glm::ivec2 &currentLocation)
{
    originalTilemapLocation_ = currentLocation;
    steps_.push(currentLocation);
    auto location1D = ConvertTo1D(currentLocation); 
    if (Tilemap::InBounds(currentLocation) && 
        tilemap_->GetTile(currentLocation).weight > 0 &&
        !visitedBitset_.test(location1D))
    {
        visitedBitset_.set(location1D, true);
    }
}

glm::ivec2 DFS::Step()
{
    const std::array<glm::ivec2, 4>* orderedDirections = nullptr;
    switch (order_)
    {
        case Order::UDLR:
        {
            constexpr std::array<glm::ivec2, 4> directions
            {
                {
                    { 0, 1 },
                    { 0, -1 },
                    { -1, 0 },
                    { 1, 0 }
                }
            };
            orderedDirections = &directions;
            break;
        }
        case Order::LDRU:
        {
            constexpr std::array<glm::ivec2, 4> directions
            {
                {
                    { -1, 0 },
                    { 0, -1 },
                    { 1, 0 },
                    { 0, 1 }
                }
            };
            orderedDirections = &directions;
            break;
        }
        case Order::Random:
        {
            static std::array<glm::ivec2, 4> directions
            {
                {
                    { -1, 0 },
                    { 0, -1 },
                    { 1, 0 },
                    { 0, 1 }
                }
            };
            std::random_shuffle(directions.begin(), directions.end());
            orderedDirections = &directions;
            break;
        }
    }

    if (!orderedDirections)
    {
        return glm::ivec2();
    }

    while (!steps_.empty())
    {
        for (auto& direction : *orderedDirections)
        {
            auto newLocation = steps_.top() + direction;
            auto newLocation1D = ConvertTo1D(newLocation);
            if (Tilemap::InBounds(newLocation) && 
                tilemap_->GetTile(newLocation).weight > 0 &&
                !visitedBitset_.test(newLocation1D))
            {
                visitedBitset_.set(newLocation1D, true);
                steps_.push(newLocation);
                return newLocation;
            }
        }
        steps_.pop();
        if (!steps_.empty())
        {
            return steps_.top();
        }
    }
    Flush();
    return steps_.top();
}

void DFS::Reset()
{
    visitedBitset_.reset();
}

void DFS::Flush()
{
    Reset();
    Init(originalTilemapLocation_);
}

uint32_t DFS::ConvertTo1D(const glm::ivec2 &currentLocation)
{
    return currentLocation.y * Tilemap::MaxHorizontalLength + currentLocation.x;
}

std::vector<glm::ivec2> DFS::GetNeighbours(const glm::ivec2 &location,
    const std::array<glm::ivec2, 4>* directions) const
{
    std::vector<glm::ivec2> neighbours;

    for (int i = 0; i < 4; ++i)
    {
        glm::ivec2 newPosition = location + (*directions)[i];

        if (tilemap_->InBounds(newPosition) &&
            tilemap_->GetTile(newPosition).weight > 0)
        {
            neighbours.emplace_back(newPosition);
        }
    }

    return neighbours;
}
