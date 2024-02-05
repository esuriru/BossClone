#pragma once

#include <glm/glm.hpp>
#include "EC/Component.h"

class TilemapEntityColor : public Component
{
public:
    TilemapEntityColor(GameObject& gameObject);

    void Update(Timestep ts) override;

    inline void SetTransformToFollow(Transform* transformToFollow)
    {
        transformToFollow_ = transformToFollow;
    }

    inline void SetOffset(const glm::vec3& offset)
    {
        offset_ = offset;
    }

    void SetColor(const glm::vec4& color);

private:
    Transform* transformToFollow_ = nullptr;            
    glm::vec3 offset_;
};