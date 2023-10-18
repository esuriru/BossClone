#pragma once
#include <glm/glm.hpp>

class Collider2D;

struct Bounds
{
private:
    Collider2D* ownerCollider_;
    glm::vec3 localExtents_ {};
    glm::vec3 center_ {};
    glm::vec3 extents_ {};

public:
    inline const glm::vec3& GetCenter() const
    {
        return center_;
    }

    inline const glm::vec3& GetExtents() const
    {
        return extents_;
    }

    void SetCenter(const glm::vec3& newCenter);
    void SetLocalExtents(const glm::vec3& localExtents);
    void SetExtents(const glm::vec3& extents);

    Bounds(Collider2D* ownerCollider, glm::vec3 center = glm::vec3(), 
        glm::vec3 extents = glm::vec3(1.0f))
        : ownerCollider_(ownerCollider)
        , center_(center)
        , extents_(extents)
        , localExtents_(extents)
    {

    }

    void UpdateScale(const glm::vec3& scale);

    bool Intersects(const Bounds& other) const;
};