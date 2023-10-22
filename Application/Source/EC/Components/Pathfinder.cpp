#include "Pathfinder.h"

#include "EC/GameObject.h"

Pathfinder::Pathfinder(GameObject &gameObject)
    : Component(gameObject)
{
    cameFrom_.resize(Tilemap::MaxSize);
    closed_.resize(Tilemap::MaxSize, false);
}

void Pathfinder::Start()
{
    tilemap_ = gameObject_.GetComponent<Tilemap>();
}

std::vector<glm::vec2> Pathfinder::BuildPath() const
{
    std::vector<glm::vec2> path;
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

bool Pathfinder::IsValid(const glm::vec2 &pos) const
{
    return pos.x >= 0 && 
        pos.x < static_cast<float>(Tilemap::MaxHorizontalLength) &&
        pos.y >= 0 && 
        pos.y < static_cast<float>(Tilemap::MaxVerticalLength);
}

bool Pathfinder::IsBlocked(const unsigned int row, const unsigned int column) const
{
    return tilemap_->GetTile(row, column).tileType == Tile::Solid;
}

void Pathfinder::ResetData()
{
    while (open_.size() != 0)
    {
        open_.pop();
    }

    for (uint32_t i = 0; i < cameFrom_.size(); ++i)
    {
        cameFrom_[i].position = glm::vec2(0.0f);
        cameFrom_[i].parent = glm::vec2(0.0f);
        cameFrom_[i].F = 0; 
        cameFrom_[i].G = 0; 
        cameFrom_[i].H = 0; 
    }

    for (uint32_t i = 0; i < closed_.size(); ++i)
    {
        closed_[i] = false;
    }
}

int Pathfinder::ConvertTo1D(const glm::vec2 &pos) const
{
    return static_cast<int>(pos.y * Tilemap::MaxHorizontalLength + pos.x);
}

std::vector<glm::vec2> Pathfinder::Pathfind(const glm::vec2 &start, const glm::vec2 &target, HeuristicFunction func, const int weight)
{
    if (IsBlocked(start.y, start.x) || IsBlocked(target.y, target.x))
    {
        CC_ERROR("Invalid start or target");
        return std::vector<glm::vec2>();
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
    glm::vec2 currentPosition;

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
    }

    open_.pop();
    closed_[ConvertTo1D(currentPosition)] = true;

    for (uint32_t i = 0; i < directionsCount_; ++i)
    {
        glm::vec2 newPosition = currentPosition + Tilemap::Directions[i];
        const uint32_t newIndex = ConvertTo1D(newPosition);

        if (!IsValid(newPosition) || IsBlocked(newPosition.y, newPosition.x)
            || closed_[newIndex])
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
    return BuildPath();
}
