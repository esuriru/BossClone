#pragma once
#include <glm/glm.hpp>

struct Bounds
{
    glm::vec3 center;
    glm::vec3 extents;

    bool Intersects(const Bounds& other) const;
};