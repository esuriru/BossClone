#pragma once

#include "Physics/Bounds.h"
#include "EC/Component.h"

class Collider2D : public Component
{
public:
    Collider2D(GameObject& gameObject);

    bool isTrigger;

    void SetOffset(const glm::vec2& offset);
    glm::vec2 GetOffset() const;

    inline const Bounds& GetBounds() const
    {
        return bounds_;
    }

private:
    Bounds bounds_;
    glm::vec2 offset_;

};