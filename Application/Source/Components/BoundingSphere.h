#pragma once

#include "BoundingVolume.h"

class Collider;

class BoundingSphere : public BoundingVolume
{
public:
	BoundingSphere(Collider* parent, float radius);

	inline bool IsIntersecting(const BoundingVolume* other) const
	{
		return other->IsIntersecting(this);
	}

	bool IsIntersecting(const TerrainVolume* other) const;
	bool IsIntersecting(const AABB* other) const;
	bool IsIntersecting(const BoundingSphere* other) const;
	bool IsIntersecting(const RayVolume* other) const override;

	float radius;
};
