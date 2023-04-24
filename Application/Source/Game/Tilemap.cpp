#include "Tilemap.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#define CC_TILEMAP_FLIP

Tilemap::Tilemap(const std::string &csv_file_path)
    : tilemapEntities_(TilemapData::TILEMAP_MAX_X_LENGTH * TilemapData::TILEMAP_MAX_Y_LENGTH)
    , isFirstInit_(true)
{
    ImportTilemapCSV(csv_file_path);
}

auto Tilemap::SetSubTexture(uint8_t tileID, const Ref<SubTexture2D> &texture, bool generateEntities) -> void
{
    subTextureMap_[tileID] = texture;

    if (generateEntities)
        GenerateEntities();
}

auto Tilemap::ImportTilemapCSV(const std::string &csv_file_path) -> void
{
    rapidcsv::Document tilemapDoc(csv_file_path);

    if ((TilemapData::TILEMAP_MAX_X_LENGTH) != static_cast<unsigned int>(tilemapDoc.GetColumnCount()) || 
        (TilemapData::TILEMAP_MAX_Y_LENGTH) != static_cast<unsigned int>(tilemapDoc.GetRowCount()))
    {
        CC_ERROR("Could not load Tilemap with file path ", csv_file_path, ". Not abiding by tilemap standards. CSV has ", tilemapDoc.GetColumnCount(), " columns and ", tilemapDoc.GetRowCount(), " rows.");
        return;
    }

    for (int i = 0; i < TilemapData::TILEMAP_MAX_Y_LENGTH; ++i)
    {
        std::vector<int> row = tilemapDoc.GetRow<int>(i);

        for (int j = 0; j < TilemapData::TILEMAP_MAX_X_LENGTH; ++j)
        {
            mapData_[i][j] = static_cast<uint8_t>(row[j]);
        }
    }
}

static Coordinator* coordinator = Coordinator::Instance();
auto Tilemap::GenerateEntities() -> void
{
    if (isFirstInit_)
        isFirstInit_ = false;
    else
    {        // Destory the existing entities.
        for (auto& e : tilemapEntities_)
        {
            coordinator->DestroyEntity(e);
        }
    }

    for (int i = 0; i < TilemapData::TILEMAP_MAX_Y_LENGTH; ++i)
    {
        for (int j = 0; j < TilemapData::TILEMAP_MAX_X_LENGTH; ++j)
        {
            if (mapData_[i][j] == 0)
                continue;

            auto entity = coordinator->CreateEntity();

            coordinator->AddComponent(
                entity,
                TransformComponent {
#ifdef CC_TILEMAP_FLIP
                    glm::vec3(j, TilemapData::TILEMAP_MAX_Y_LENGTH - i, 0)
#else
                    glm::vec3(j, i, 0)
#endif
                });

            coordinator->AddComponent(
                entity,
                TileRendererComponent{
                    subTextureMap_[mapData_[i][j]]
                });

            tilemapEntities_.push_back(entity);
        }
    }
}
