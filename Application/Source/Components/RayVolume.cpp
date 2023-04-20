#include "RayVolume.h"
#include "Collider.h"
#include "BoundingSphere.h"

RayVolume::RayVolume(Collider* c)
	: BoundingVolume(c)
{
}

bool RayVolume::IsIntersecting(const TerrainVolume* other) const
{
	return false;
}

bool RayVolume::IsIntersecting(const AABB* other) const
{
	return false;
}

bool RayVolume::IsIntersecting(const BoundingSphere* other) const
{
	return other->IsIntersecting(this);
}

bool RayVolume::IsIntersecting(const RayVolume* other) const
{
	return false;
}

