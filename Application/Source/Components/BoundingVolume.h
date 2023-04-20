#pragma once
#include "Collider.h"

class AABB;
class TerrainVolume;
class BoundingSphere;
class RayVolume;

class BoundingVolume
{
public:
	BoundingVolume(Collider* parent);

	virtual bool IsIntersecting(const BoundingVolume* other) const = 0;
	virtual bool IsIntersecting(const TerrainVolume* other) const = 0;
	virtual bool IsIntersecting(const AABB* other) const = 0;
	virtual bool IsIntersecting(const BoundingSphere* other) const = 0;
	virtual bool IsIntersecting(const RayVolume* other) const = 0;

	Collider* GetAttachedCollider() const;
	virtual void SetPosition(const glm::vec3& pos);
	virtual void SetPosition(const glm::vec2& pos);

	inline const glm::vec3& GetPosition() const
	{
		return centre_;
	}


protected:
	Collider* attachedCollider_;
	glm::vec3 centre_;

	void AttachCollision(Collision& col) const;

};

