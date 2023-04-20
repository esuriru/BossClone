#include "SphereCollider.h"
#include "RigidBody.h"
#include "BoundingSphere.h"
#include "Scene/GameObject.h"
#include "Transform.h"

SphereCollider::SphereCollider(GameObject* go)
	: Collider(go)
{
	vol_ = new BoundingSphere(this, go->GetTransform()->GetScale().x);
	boundingVolume = vol_;
	boundingVolume->SetPosition(go->GetTransform()->GetPosition());
}

void SphereCollider::Update(double dt)
{

	if (isDirty_)
	{
		// Should not be here.
		attachedRigidbody = gameObject_->GetComponent<RigidBody>();

		boundingVolume->SetPosition(gameObject_->GetTransform()->GetWorldPosition() + offset_);
		isDirty_ = false;
	}

	if (!attachedRigidbody)
	{
		boundingVolume->SetPosition(gameObject_->GetTransform()->GetWorldPosition() + offset_);
	}
}

void SphereCollider::SetRadius(float radius)
{
	// TODO - Not make this a dynamic cast
	vol_->radius = radius;
}
