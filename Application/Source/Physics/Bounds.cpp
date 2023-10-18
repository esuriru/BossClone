#include "Bounds.h"

void Bounds::SetCenter(glm::vec3 newCenter)
{
    center_ = newCenter;
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
