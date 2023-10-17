#include "Bounds.h"

bool Bounds::Intersects(const Bounds& other) const
{
    if (extents.x == 0.f || extents.y == 0.f || other.extents.x == 0.f || other.extents.y == 0.f ||
        fabs((center.x) - (other.center.x)) > extents.x + other.extents.x ||
            fabs((center.y) - (other.center.y)) > extents.y + other.extents.y)
                return false;
    return true;
}
