#include "BoxCollider.h"
#include "RigidBody.h"
#include "AABB.h"
#include "Scene/GameObject.h"
#include "Transform.h"
#include "Core/Log.h"
#include <glm/gtx/string_cast.hpp>

BoxCollider::BoxCollider(GameObject* go)
    :Collider(go)
{
    vol_ = new AABB(this, go->GetTransform()->GetPosition(), go->GetTransform()->GetScale());
    boundingVolume = vol_;
    boundingVolume->SetPosition(go->GetTransform()->GetPosition());
}

void BoxCollider::Update(double dt)
{
    if (isDirty_)
    {
        // Should not be here.
        attachedRigidbody = gameObject_->GetComponent<RigidBody>();

        boundingVolume->SetPosition(gameObject_->GetTransform()->GetWorldPosition() + offset_);
        isDirty_ = false;
    }

    //CC_INFO(glm::to_string(boundingVolume->GetPosition()));
}

void BoxCollider::SetSize(glm::vec3 size)
{
    vol_->size = size;
}

glm::vec3 BoxCollider::GetSize() const
{
    return vol_->size;
}
