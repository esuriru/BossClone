#include "Tilemap.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"


TilemapComponent::TilemapComponent(const std::string &csv_file_path)
{
    ImportTilemapCSV(csv_file_path);
}

auto TilemapComponent::ImportTilemapCSV(const std::string &csv_file_path) -> void
{
    rapidcsv::Document tilemapDoc(csv_file_path);

    if ((TilemapData::TILEMAP_MAX_X_LENGTH) != static_cast<unsigned int>(tilemapDoc.GetColumnCount()) || 
        (TilemapData::TILEMAP_MAX_Y_LENGTH) != static_cast<unsigned int>(tilemapDoc.GetRowCount()))
    {
        CC_ERROR("Could not load TilemapComponent with file path ", csv_file_path, ". Not abiding by tilemap standards. CSV has ", tilemapDoc.GetColumnCount(), " columns and ", tilemapDoc.GetRowCount(), " rows.");
        return;
    }

    for (int i = 0; i < TilemapData::TILEMAP_MAX_Y_LENGTH; ++i)
    {
        std::vector<int> row = tilemapDoc.GetRow<int>(i);

        for (int j = 0; j < TilemapData::TILEMAP_MAX_X_LENGTH; ++j)
        {
            MapData[i][j] = static_cast<uint8_t>(row[j]);
        }
    }
}

