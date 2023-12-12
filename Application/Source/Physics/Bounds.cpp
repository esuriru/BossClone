#include "Bounds.h"

#include "EC/Components/Collider2D.h"
#include "EC/Components/Transform.h"

void Bounds::SetCenter(const glm::vec3& newCenter)
{
    center_ = newCenter;
}

void Bounds::SetLocalExtents(const glm::vec3 &localExtents)
{
    localExtents_ = localExtents;
    extents_ = localExtents_ * ownerCollider_->GetTransform().GetScale();
}

void Bounds::SetExtents(const glm::vec3 &extents)
{
    extents_ = extents; 
}

void Bounds::UpdateScale(const glm::vec3 &scale)
{
    extents_ = localExtents_ * scale;
}

bool Bounds::Intersects(const Bounds &other) const
{
    if (extents_.x == 0.f || extents_.y == 0.f || other.extents_.x == 0.f || other.extents_.y == 0.f ||
        fabs((center_.x) - (other.center_.x)) > extents_.x + other.extents_.x ||
            fabs((center_.y) - (other.center_.y)) > extents_.y + other.extents_.y)
                return false;
    return true;
}

bool Bounds::IsPointInside(const glm::vec3 &point) const
{
    glm::vec3 min = center_ - extents_;
    glm::vec3 max = center_ + extents_;
    return (min.x <= point.x && point.x <= max.x && min.y <= point.y && point.y <= max.y);
}
