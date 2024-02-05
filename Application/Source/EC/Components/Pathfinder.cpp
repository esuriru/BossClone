#include "Pathfinder.h"

#include "EC/GameObject.h"

Pathfinder::Pathfinder(GameObject &gameObject)
    : Component(gameObject)
    , directionsCount_(4)
{
    cameFrom_.resize(Tilemap::MaxSize);
    closed_.resize(Tilemap::MaxSize, false);
}

void Pathfinder::Start()
{
    tilemap_ = gameObject_.GetComponent<Tilemap>();
}

std::vector<glm::ivec2> Pathfinder::BuildPath() const
{
    std::vector<glm::ivec2> path;
    auto currentPosition = target_;
    auto index = ConvertTo1D(currentPosition);

    while (!(cameFrom_[index].parent == currentPosition))
    {
        path.push_back(currentPosition);
        currentPosition = cameFrom_[index].parent;
        index = ConvertTo1D(currentPosition);
    }

    if (path.size() == 1)
    {
        if (directionsCount_ == 4)
        {
            if (fabs(target_.y - start_.y) + fabs(target_.x - start_.x) > 1)
            {
                path.clear();
            }
        }
        else
        {
            if (fabs(target_.y - start_.y) + fabs(target_.x - start_.x) > 2)
            {
                path.clear();
            }
            else if (fabs(target_.y - start_.y) + fabs(target_.x - start_.x) > 1)
            {
                path.clear();
            }
        }
    }
    else
    {
        std::reverse(path.begin(), path.end());
    }

    return path;
}

bool Pathfinder::IsValid(const glm::ivec2 &pos) const
{
    return tilemap_->InBounds(pos);
}

bool Pathfinder::IsBlocked(const unsigned int row, const unsigned int column) const
{
    return tilemap_->GetTile(row, column).weight <= 0;
}

void Pathfinder::ResetData()
{
    while (open_.size() != 0)
    {
        open_.pop();
    }

    for (uint32_t i = 0; i < cameFrom_.size(); ++i)
    {
        cameFrom_[i].position = glm::ivec2();
        cameFrom_[i].parent = glm::ivec2();
        cameFrom_[i].F = 0; 
        cameFrom_[i].G = 0; 
        cameFrom_[i].H = 0; 
    }

    for (uint32_t i = 0; i < closed_.size(); ++i)
    {
        closed_[i] = false;
    }
}

int Pathfinder::ConvertTo1D(const glm::ivec2 &pos) const
{
    return static_cast<int>(pos.y * Tilemap::MaxHorizontalLength + pos.x);
}

std::vector<glm::ivec2> Pathfinder::Pathfind(const glm::ivec2 &start, 
    const glm::ivec2 &target, HeuristicFunction func, const int weight)
{
    if (IsBlocked(start.y, start.x) || 
        IsBlocked(target.y, target.x))
    {
        CC_ERROR("Invalid start or target");
        return std::vector<glm::ivec2>();
    }

    start_ = start;
    target_ = target;
    weight_ = weight;
    heuristic_ = std::bind(func, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3);

    ResetData();

    cameFrom_[ConvertTo1D(start_)].parent = start_;
    open_.push(Tile(start_, 0));

    uint32_t newF, newG, newH;
    glm::ivec2 currentPosition;

    while (!open_.empty())
    {
        currentPosition = open_.top().position;

        if (currentPosition == target_)
        {
            while (open_.size() != 0)
            {
                open_.pop();
            }
            break;
        }

        open_.pop();
        closed_[ConvertTo1D(currentPosition)] = true;

        for (uint32_t i = 0; i < directionsCount_; ++i)
        {
            constexpr std::array<glm::ivec2, 4> Directions
            {
                {
                    { 0, 1 },
                    { 1, 0 },
                    { 0,-1 },
                    { -1,0 },
                }
            };

            glm::ivec2 newPosition = currentPosition + Directions[i];
            const uint32_t newIndex = ConvertTo1D(newPosition);

            if (!IsValid(newPosition) || IsBlocked(newPosition.y, 
                newPosition.x) || closed_[newIndex])
            {
                continue;
            }

            newG = cameFrom_[ConvertTo1D(currentPosition)].G + 1;
            newH = heuristic_(newPosition, target_, weight_);
            newF = newG + newH;

            if (cameFrom_[newIndex].F == 0 || newF < cameFrom_[newIndex].F)
            {
                open_.push(Tile(newPosition, newF));
                cameFrom_[newIndex] = Tile(newPosition, 
                    currentPosition, newF, newG, newH);
            }
        }
    }
    return BuildPath();
}
