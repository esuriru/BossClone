#pragma once

#include "Physics/Bounds.h"
#include "EC/Component.h"

class Collider2D : public Component
{
public:
    Collider2D(GameObject& gameObject);

    void OnDestroy() override; 

    bool isTrigger = true;

    void SetOffset(const glm::vec2& offset);
    glm::vec2 GetOffset() const;

    inline Bounds& GetBounds()
    {
        return bounds_;
    }

private:
    Bounds bounds_;
    glm::vec2 offset_;

    void UpdateBounds(Transform& transform);

};