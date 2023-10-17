#include "Collider2D.h"

#include "EC/Components/Transform.h"

Collider2D::Collider2D(GameObject & gameObject)
    : Component(gameObject)
{

}

void Collider2D::SetOffset(const glm::vec2 &offset)
{
    offset_ = offset;
    bounds_.center = GetTransform().GetPosition() + 
        glm::vec3(offset_, 0);
}

glm::vec2 Collider2D::GetOffset() const
{
    return offset_;
}
