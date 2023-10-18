#include "Collider2D.h"

#include "EC/Components/Transform.h"
#include "Physics/PhysicsWorld.h"
#include "Core/Core.h"

#include <glm/gtx/string_cast.hpp>

Collider2D::Collider2D(GameObject & gameObject)
    : Component(gameObject)
    , offset_()
    , bounds_(this)
{
    PhysicsWorld::Instance()->AddCollider(this);
    GetTransform()
        .AddCallback(this, std::bind(&Collider2D::UpdateBounds, 
        this, std::placeholders::_1));

    UpdateBounds(GetTransform());
}

void Collider2D::OnDestroy()
{
    PhysicsWorld::Instance()->RemoveCollider(this);
    GetTransform().RemoveCallback(this);
}

void Collider2D::SetOffset(const glm::vec2 &offset)
{
    offset_ = offset;
    bounds_.SetCenter(GetTransform().GetPosition() + 
        glm::vec3(offset_, 0));
}

glm::vec2 Collider2D::GetOffset() const
{
    return offset_;
}

void Collider2D::UpdateBounds(Transform &transform)
{
    bounds_.UpdateScale(transform.GetScale());
    bounds_.SetCenter(transform.GetPosition() + 
        glm::vec3(offset_, 0));
    // CC_TRACE(glm::to_string(bounds_.GetExtents()));
}
