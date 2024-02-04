#pragma once

#include "EC/Component.h"
#include "Tilemap.h"

#include <glm/glm.hpp>

#include <functional>
#include <vector>

class TilemapEntity : public Component
{
public:
    TilemapEntity(GameObject& gameObject);

    void Start() override;
    void Update(Timestep ts) override;
    virtual void StartTurn();

    inline virtual TilemapEntity* SetTilemap(Ref<Tilemap> tilemap)
    {
        tilemap_ = tilemap;
        return this;
    }

    void FixTilemapPosition();
    void QueueMove(glm::vec3 targetPosition, float seconds = 1.0f,
        std::function<void()> callback = nullptr);

    inline virtual TilemapEntity* SetVisibilityTilemap(Ref<Tilemap> tilemap)
    {
        visibilityTilemap_ = tilemap;
        return this;
    }

    inline virtual TilemapEntity* SetVisibilityRange(uint8_t range)
    {
        visibilityRange_ = range;
        return this;
    }
    void UpdateNearbyTilesVisibility();

private:
    glm::vec3 targetPosition_;
    glm::vec3 originalPosition_;

protected:
    void SetNearbyTilesVisible(const glm::ivec2& location, bool visible = true);
    std::vector<std::reference_wrapper<Tile>> GetNearbyTiles(
        const glm::ivec2& location, uint8_t range);

    glm::ivec2 tilemapPosition_;
    uint8_t visibilityRange_ = 3;
    float movementTime_ = 0.5f;
    Ref<Tilemap> visibilityTilemap_;

    std::function<void()> moveCallback_;

    bool isMoving_;
    bool isCurrentTurn_;

    Ref<Tilemap> tilemap_ = nullptr;
    float timer_;
    float moveTime_;
    float inverseMoveTime_;

};