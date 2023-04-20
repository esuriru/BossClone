#include "BoundingVolume.h"

BoundingVolume::BoundingVolume(Collider* parent)
	: attachedCollider_(parent)
{
}


Collider* BoundingVolume::GetAttachedCollider() const
{
	return attachedCollider_;
}

void BoundingVolume::SetPosition(const glm::vec3& pos)
{
	centre_ = pos;
}

void BoundingVolume::SetPosition(const glm::vec2& pos)
{
	centre_ = glm::vec3(pos, 0);
}

void BoundingVolume::AttachCollision(Collision& col) const
{
	if (col.rigidbody || col.otherRigidbody)
	{
		if (col.rigidbody)
		{
			if (col.collider->unhandledCollisions.find(col.otherCollider) == col.collider->unhandledCollisions.end())
				col.collider->unhandledCollisions[col.otherCollider] = col;
			return;
		}
		else
		{
			if (col.otherCollider->unhandledCollisions.find(col.collider) == col.otherCollider->unhandledCollisions.end())
				col.otherCollider->unhandledCollisions[col.collider] = col;
			return;
		}
	}
}
