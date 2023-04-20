#pragma once
#include "BoundingVolume.h"

class RayVolume : public BoundingVolume
{
public:
	RayVolume(Collider* c);

	inline bool IsIntersecting(const BoundingVolume* other) const override
	{
		return other->IsIntersecting(this);
	}

	bool IsIntersecting(const TerrainVolume* other) const override;
	bool IsIntersecting(const AABB* other) const override;
	bool IsIntersecting(const BoundingSphere* other) const override;
	bool IsIntersecting(const RayVolume* other) const override;

	glm::vec3 direction;
};


