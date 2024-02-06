#pragma once

#include "EC/Component.h"
#include "Tilemap.h"

#include <glm/glm.hpp>

#include <functional>
#include <vector>
#include "TilemapEntityColor.h"

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
        std::function<void()> callback = nullptr, int turnsToLock = 0);

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

    inline virtual TilemapEntity* SetStartHealth(float health)
    {
        initialHealth_ = currentHealth_ = health;
        return this;
    }

    inline virtual TilemapEntity* SetColorRepresentation(const glm::vec4& color)
    {
        colorRepresentation_ = color;
        colorObject_->SetColor(color);
        return this;
    }

    inline virtual TilemapEntity* SetColorObject(Ref<TilemapEntityColor> color)
    {
        colorObject_ = color;
        return this;
    }

    inline virtual const glm::ivec2& GetTilemapPosition() const
    {
        return tilemapPosition_;
    }

    inline float GetHealth() const
    {
        return currentHealth_;
    }

    inline const glm::vec4& GetColourRepresentation() const
    {
        return colorRepresentation_;
    }

    inline Ref<TilemapEntityColor> GetColorObject() const
    {
        return colorObject_;
    }

    inline bool IsMoving() const
    {
        return isMoving_;
    }

    virtual void UpdateNearbyTilesVisibility();
    void TakeDamage(float amount);

private:
    glm::vec3 targetPosition_;
    glm::vec3 originalPosition_;
    glm::vec4 colorRepresentation_;

protected:
    int turnsLocked_ = 0;

    float initialHealth_ = 100.0f;
    float currentHealth_ = 100.0f;

    glm::ivec2 tilemapPosition_;
    uint8_t visibilityRange_ = 3;
    float movementTime_ = 0.35f;
    Ref<Tilemap> visibilityTilemap_;

    std::function<void()> moveCallback_;

    bool isMoving_;
    bool isCurrentTurn_;

    Ref<Tilemap> tilemap_ = nullptr;
    Ref<TilemapEntityColor> colorObject_ = nullptr;
    float timer_;
    float moveTime_;
    float inverseMoveTime_;

    virtual void OnDeath() {}

    virtual void SetNearbyTilesVisible(const glm::ivec2& location, 
        bool visible = true);

    std::vector<std::reference_wrapper<Tile>> GetNearbyTiles(
        const glm::ivec2& location, uint8_t range);
    std::vector<glm::ivec2> GetNearbyTileLocations(
        const glm::ivec2& location, uint8_t range);
};