#pragma once

#include "EC/Component.h"
#include "Tilemap.h"

#include <glm/glm.hpp>

#include <functional>

class TilemapEntity : public Component
{
public:
    TilemapEntity(GameObject& gameObject);

    void Start() override;
    void Update(Timestep ts) override;

    inline void SetTilemap(Ref<Tilemap> tilemap)
    {
        tilemap_ = tilemap;
    }

    void FixTilemapPosition();
    void QueueMove(glm::vec3 targetPosition, float seconds = 1.0f,
        std::function<void()> callback = nullptr);

protected:
    glm::ivec2 tilemapPosition_;
    glm::vec3 targetPosition_;
    glm::vec3 originalPosition_;

    std::function<void()> moveCallback_;

    bool isMoving_;

    Ref<Tilemap> tilemap_ = nullptr;
    float timer_;
    float moveTime_;
    float inverseMoveTime_;

};