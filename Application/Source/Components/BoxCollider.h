#pragma once

#include "BoundingVolume.h"
#include <glm/glm.hpp>
#include "Collider.h"
#include "AABB.h"

class BoxCollider : public Collider
{
public:
    BoxCollider(GameObject* go);
    void Update(double dt);
    void SetSize(glm::vec3 size);
    glm::vec3 GetSize() const;
    AABB* vol_;
};
