#include "BoundingVolume2D.h"
#include "RigidBody2D.h"
#include "Collider2D.h"



BoundingVolume2D::BoundingVolume2D()
	: centre()
	, size()
{
}

BoundingVolume2D::BoundingVolume2D(Collider2D* attached, const glm::vec2& centre, const glm::vec2& size)
	: attachedToCollider_(attached)
	, centre(centre)
	, size(size)
{
}

void BoundingVolume2D::Rotate(const glm::quat& rot)
{

}

void BoundingVolume2D::SetPosition(const glm::vec3& pos)
{
	// Might need to add the offset to here
	centre = pos;
}

void BoundingVolume2D::SetPosition(const glm::vec2& pos)
{
	centre = glm::vec3(pos, 0);
}


Collider2D* BoundingVolume2D::GetAttachedCollider() const
{
	return attachedToCollider_;
}

void BoundingVolume2D::AttachCollision(Collision2D& col) const
{
	if (col.rigidbody || col.otherRigidbody)
	{
		if (col.rigidbody)
		{
			if (attachedToCollider_->unhandledCollisions.find(col.otherCollider) == attachedToCollider_->unhandledCollisions.end())
				attachedToCollider_->unhandledCollisions[col.otherCollider] = col;
			return;
		}
		else
		{
			if (col.otherCollider->unhandledCollisions.find(attachedToCollider_) == col.otherCollider->unhandledCollisions.end())
				col.otherCollider->unhandledCollisions[attachedToCollider_] = col;
			return;
		}
	}
}


