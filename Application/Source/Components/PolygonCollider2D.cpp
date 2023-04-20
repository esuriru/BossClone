#include "PolygonCollider2D.h"

#include "Scene/GameObject.h"
#include "OBB2D.h"

PolygonCollider2D::PolygonCollider2D(GameObject* go)
	: Collider2D(go)
	, numberOfVertices_(0)
{
}


void PolygonCollider2D::Update(double dt)
{
	if (isDirty_)
	{
		RetrieveRigidBody();
		isDirty_ = false;
	}

	if (attachedRigidbody)
		position_ = targetTransform_->GetPosition();

	if (localRotation_ != targetTransform_->GetRotation().z)
	{
		localRotation_ = targetTransform_->GetRotation().z;
		Rotate(localRotation_);
	}
}

void PolygonCollider2D::SetToTriangleCollider()
{
	numberOfVertices_ = 3;
	bounds_ = OBB2D::CreateTriangleOBB(this, gameObject_->GetTransform()->GetPosition(), gameObject_->GetTransform()->GetScale());
	boundingVolume = bounds_;
}

void PolygonCollider2D::Rotate(float clockwiseRotationDegrees)
{
	bounds_->RotateVertices(clockwiseRotationDegrees);
}
