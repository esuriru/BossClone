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
