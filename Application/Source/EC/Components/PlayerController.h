#pragma once

#include "TilemapEntity.h"

#include <vector>

class PlayerController : public TilemapEntity 
{
public:
    PlayerController(GameObject& gameObject);

    void Start() override;
    void Update(Timestep ts) override;

    inline PlayerController* SetVisibilityTilemap(Ref<Tilemap> tilemap)
    {
        visibilityTilemap_ = tilemap;
        return this;
    }

    inline PlayerController* SetVisibilityRange(uint8_t range)
    {
        visibilityRange_ = range;
        return this;
    }

    void SetNearbyTilesVisible(const glm::ivec2& location, bool visible = true);
private:
    uint8_t visibilityRange_ = 3;
    float movementTime_ = 0.5f;
    Ref<Tilemap> visibilityTilemap_;

    std::vector<std::reference_wrapper<Tile>> GetNearbyTiles(
        const glm::ivec2& location, uint8_t range);
};