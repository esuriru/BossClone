#include "Tilemap.h"
#include "TilemapRenderer.h"
#include "rapidcsv.h"
#include "EC/GameObject.h"

Tilemap::Tilemap(GameObject& gameObject)
    : Component(gameObject)
    , tileSize(16.0f)
    , tilingFactor(1.0f)
{
    Ref<TilemapRenderer> tilemapRenderer = gameObject.AddComponent<TilemapRenderer>();
    tilemapRenderer->SetTilemapComponent(this);
}

auto CharToTileType(char csv_input) -> decltype(Tile::Empty) 
{
    switch (csv_input)
    {
        case 'S': return Tile::Solid;
        case 'P': return Tile::OneWay;
        default: break;
    }
    return Tile::Empty;
}

void Tilemap::LoadCSV(const std::string& textureCsvFilePath, const std::string& typeCsvFilePath)
{
    rapidcsv::Document tilemapDoc(textureCsvFilePath);
    rapidcsv::Document tilemapTypesDoc(typeCsvFilePath);

    if (
            (MaxHorizontalLength) != static_cast<unsigned int>(tilemapDoc.GetColumnCount()) || 
            (MaxVerticalLength) != static_cast<unsigned int>(tilemapDoc.GetRowCount()) || 
            (MaxHorizontalLength) != static_cast<unsigned int>(tilemapTypesDoc.GetColumnCount()) || 
            (MaxVerticalLength) != static_cast<unsigned int>(tilemapTypesDoc.GetRowCount())
        )
    {
        CC_FATAL("Error loading either one of the CSV files (I'm too lazy to write the whole thing down and this is gonna kill me when I debug).");
        return;
    }

    for (int i = 0; i < MaxVerticalLength; ++i)
    {
        std::vector<int> mapRow = tilemapDoc.GetRow<int>(i);
        std::vector<char> typeRow = tilemapTypesDoc.GetRow<char>(i);

        for (int j = 0; j < MaxHorizontalLength; ++j)
        {
            const size_t index_x = (MaxVerticalLength - 1) - i;

            tileData_[index_x][j].textureIndex = static_cast<uint8_t>(mapRow[j]);
            tileData_[index_x][j].tileType = CharToTileType(typeRow[j]);
        }
    }
    
}

Tile &Tilemap::GetTile(uint32_t x, uint32_t y)
{
    return tileData_[x][y];
}

Ref<SubTexture2D> &Tilemap::GetTexture(uint32_t index)
{
    return subTextureMap_[index];
}
