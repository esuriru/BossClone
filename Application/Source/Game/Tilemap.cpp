#include "Tilemap.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"


TilemapComponent::TilemapComponent(const std::string &map_csv, bool make_tiles_solid)
{
    ImportTilemapCSV(map_csv, make_tiles_solid);
    InitializeQuadtree();
}

TilemapComponent::TilemapComponent(const std::string &map_csv, const char* tile_type_csv) 
{
    TilemapPath = map_csv;
    TilemapTypesPath = std::string(tile_type_csv);
    ImportTilemapCSV(map_csv, TilemapTypesPath);
    InitializeQuadtree();
}

auto TilemapComponent::ImportTilemapCSV(const std::string &map_csv, bool make_tiles_solid) -> void
{
    rapidcsv::Document tilemapDoc(map_csv);

    if ((TilemapData::TILEMAP_MAX_X_LENGTH) != static_cast<unsigned int>(tilemapDoc.GetColumnCount()) || 
        (TilemapData::TILEMAP_MAX_Y_LENGTH) != static_cast<unsigned int>(tilemapDoc.GetRowCount()))
    {
        CC_ERROR("Could not load TilemapComponent with file path ", map_csv, ". Not abiding by tilemap standards. CSV has ", tilemapDoc.GetColumnCount(), " columns and ", tilemapDoc.GetRowCount(), " rows.");
        return;
    }

    for (int i = 0; i < TilemapData::TILEMAP_MAX_Y_LENGTH; ++i)
    // for (int i = TilemapData::TILEMAP_MAX_Y_LENGTH - 1; i > 0; --i)
    {
        std::vector<int> row = tilemapDoc.GetRow<int>(i);

        for (int j = 0; j < TilemapData::TILEMAP_MAX_X_LENGTH; ++j)
        {
            const size_t index_x = (TilemapData::TILEMAP_MAX_Y_LENGTH - 1) - i;
            MapData[index_x][j].TextureIndex = static_cast<uint8_t>(row[j]);
            if (make_tiles_solid && row[j] > 0)
                MapData[index_x][j].Type = Tile::TileType::Solid;
        }
    }
}

auto CharToTileType(char csv_input) -> Tile::TileType
{
    switch (csv_input)
    {
        case 'S': return Tile::TileType::Solid;
        case 'P': return Tile::TileType::OneWay;
        default: break;
    }
    return Tile::TileType::Empty;
}

auto TilemapComponent::ImportTilemapCSV(const std::string &map_csv, const std::string &tile_type_csv) -> void
{
    rapidcsv::Document tilemapDoc(map_csv);
    rapidcsv::Document tilemapTypesDoc(tile_type_csv);

    if (
            (TilemapData::TILEMAP_MAX_X_LENGTH) != static_cast<unsigned int>(tilemapDoc.GetColumnCount()) || 
            (TilemapData::TILEMAP_MAX_Y_LENGTH) != static_cast<unsigned int>(tilemapDoc.GetRowCount()) || 
            (TilemapData::TILEMAP_MAX_X_LENGTH) != static_cast<unsigned int>(tilemapTypesDoc.GetColumnCount()) || 
            (TilemapData::TILEMAP_MAX_Y_LENGTH) != static_cast<unsigned int>(tilemapTypesDoc.GetRowCount())
        )
    {
        CC_FATAL("Error loading either one of the CSV files (I'm too lazy to write the whole thing down and this is gonna kill me when I debug).");
        return;
    }

    for (int i = 0; i < TilemapData::TILEMAP_MAX_Y_LENGTH; ++i)
    // for (int i = TilemapData::TILEMAP_MAX_Y_LENGTH - 1; i > 0; --i)
    {
        std::vector<int> mapRow = tilemapDoc.GetRow<int>(i);
        std::vector<char> typeRow = tilemapTypesDoc.GetRow<char>(i);

        for (int j = 0; j < TilemapData::TILEMAP_MAX_X_LENGTH; ++j)
        {
            const size_t index_x = (TilemapData::TILEMAP_MAX_Y_LENGTH - 1) - i;

            MapData[index_x][j].TextureIndex = static_cast<uint8_t>(mapRow[j]);
            MapData[index_x][j].Type = CharToTileType(typeRow[j]);
        }
    }
    
}

auto TilemapComponent::InitializeQuadtree() -> void
{
    HorizontalAreasCount = glm::ceil(
        static_cast<float>(TilemapData::TILEMAP_MAX_X_LENGTH) *
            TileSize.x / static_cast<float>(QuadtreeGridAreaWidth));

    VerticalAreasCount = glm::ceil(
        static_cast<float>(TilemapData::TILEMAP_MAX_Y_LENGTH) *
            TileSize.y / static_cast<float>(QuadtreeGridAreaHeight));

    // 2D vector resize
    ObjectsInArea.resize(HorizontalAreasCount);
    for (int i = 0; i < HorizontalAreasCount; ++i)
    {
        ObjectsInArea[i].resize(VerticalAreasCount);
    }    
    CC_TRACE("Horizontal area count = ", HorizontalAreasCount, ", Vertical area count = ", VerticalAreasCount);
}

std::vector<glm::vec2> TilemapComponent::BuildPath() const
{
    std::vector<glm::vec2> path;
    auto cp = _target;
    auto index = ConvertTo1D(cp);

    while (!(_cameFrom[index].Parent == cp))
    {
        path.push_back(cp);
        cp = _cameFrom[index].Parent;
        index = ConvertTo1D(cp);
    }

    if (path.size() == 1)
    {
        if (DirectionsCount == 4)
        {
            if (fabs(_target.y - _start.y) + fabs(_target.x - _start.x) > 1)
            {
                path.clear();
            }
        }
        else
        {
            if (fabs(_target.y - _start.y) + fabs(_target.x - _start.x) > 2)
            {
                path.clear();
            }
            else if (fabs(_target.y - _start.y) + fabs(_target.x - _start.x) > 1)
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

bool TilemapComponent::IsValid(const glm::vec2 &pos) const
{
    return (pos.x >= 0) && (pos.x < TilemapData::TILEMAP_MAX_X_LENGTH) &&
        (pos.y >= 0) && (pos.y < TilemapData::TILEMAP_MAX_Y_LENGTH);
}

bool TilemapComponent::IsBlocked(const unsigned int row, const unsigned int col, const bool invert) const
{
	if (invert)
	{
		return ((MapData[TilemapData::TILEMAP_MAX_Y_LENGTH - row - 1][col].Type == Tile::Solid));
	}
	else
	{
		return ((MapData[row][col].Type == Tile::Solid));
	}
}

unsigned int Heuristic::Manhattan(const glm::vec2 &v1, const glm::vec2 &v2, int weight)
{
    glm::vec2 delta = v2 - v1;
	return static_cast<unsigned int>(weight * (delta.x + delta.y));
}

unsigned int Heuristic::Euclidean(const glm::vec2 &v1, const glm::vec2 &v2, int weight)
{
    glm::vec2 delta = v2 - v1;
	return static_cast<unsigned int>(weight * sqrt((delta.x * delta.x) + (delta.y * delta.y)));
}

std::vector<glm::vec2> TilemapComponent::Pathfind(const glm::vec2 &start, const glm::vec2 &target, HeuristicFunction func, const int weight)
{
    if (IsBlocked(start.x, start.y) || IsBlocked(target.x, target.y))
    {
        CC_ERROR("Invalid start or target.");
        return std::vector<glm::vec2>();
    }

    _start = start;
    _target = target;
    _weight = weight;
    _heuristic = std::bind(_heuristic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    ResetAStarLists();

    _cameFrom[ConvertTo1D(_start)].Parent = _start;
    _open.push(Tile(_start, 0));

    unsigned int newF, newG, newH;
    glm::vec2 currentPosition;

    while (_open.empty())
    {
        currentPosition = _open.top().Position;

        if (currentPosition == _target)
        {
            while (_open.size() != 0)
            {
                _open.pop(); 
            }
            break;
        }

        _open.pop();
        _closed[ConvertTo1D(currentPosition)] = true;

        for (unsigned int i = 0; i < DirectionsCount; ++i)
        {
            auto np = currentPosition + TilemapData::DIRECTIONS[i];
            const auto ni = ConvertTo1D(np);

            if (!IsValid(np) || IsBlocked(np.x, np.y) || _closed[ni])
            {
                continue;
            }

            newG = _cameFrom[ConvertTo1D(currentPosition)].G + 1;
            newH = _heuristic(np, _target, _weight);
            newF = newG + newH;

            if (_cameFrom[ni].F == 0 || newF < _cameFrom[ni].F)
            {
                _open.push(Tile(np, newF));
                _cameFrom[ni] = { np, currentPosition, newF, newG, newH };
            }
        }
    }
    return BuildPath();
}

void TilemapComponent::ResetAStarLists()
{
    // Delete m_openList
	while (_open.size() != 0)
		_open.pop();
	// Reset m_cameFromList
	for (int i = 0; i < _cameFrom.size(); i++)
	{
		_cameFrom[i].Position = glm::vec2(0,0);
		_cameFrom[i].Parent = glm::vec2(0, 0);
		_cameFrom[i].F = 0;
		_cameFrom[i].G = 0;
		_cameFrom[i].H = 0;
	}
	// Reset m_closedList
	for (int i = 0; i < _closed.size(); i++)
	{
		_closed[i] = false;
	}

}

int TilemapComponent::ConvertTo1D(const glm::vec2 &pos) const
{
    return (pos.y * TilemapData::TILEMAP_MAX_X_LENGTH) + pos.x;
}
