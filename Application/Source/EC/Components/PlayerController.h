#pragma once

#include "TilemapEntity.h"

#include <vector>

class PlayerController : public TilemapEntity 
{
public:
    PlayerController(GameObject& gameObject);

    void Start() override;
    void Update(Timestep ts) override;

    inline PlayerController* SetTilemap(Ref<Tilemap> tilemap) override
    {
        tilemap_ = tilemap;
        return this;
    }

    inline PlayerController* SetVisibilityTilemap(Ref<Tilemap> tilemap) override
    {
        visibilityTilemap_ = tilemap;
        return this;
    }

    inline PlayerController* SetVisibilityRange(uint8_t range) override
    {
        visibilityRange_ = range;
        return this;
    }

private:
    bool TestForWin(const glm::ivec2& location);
    float baseDamage_ = 5.0f;

protected:
    void OnDeath() override;
};