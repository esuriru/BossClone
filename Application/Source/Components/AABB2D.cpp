#include "AABB2D.h"

#include "BoundingCircle.h"
#include "OBB2D.h"

bool AABB2D::IsIntersecting(const BoundingVolume2D* bv) const
{
	return bv->IsIntersecting(this);
}

AABB2D::AABB2D()
	: BoundingVolume2D()
{
}

AABB2D::AABB2D(Collider2D* attached, const glm::vec2& centre, const glm::vec2& size)
	: BoundingVolume2D(attached, centre, size)
{

}

bool AABB2D::IsIntersecting(const OBB2D* bv) const
{
	return bv->IsIntersecting(this);
}

bool AABB2D::IsIntersecting(const AABB2D* bv) const
{
	OBB2D converted = OBB2D(bv);
	return OBB2D(this).IsIntersecting(&converted);
#if 0
	return 
		this->centre.x  < bv->centre.x  + bv->size.x &&
		this->centre.x  + this->size.x > bv->centre.x &&
		this->centre.y  < bv->centre.y + bv->size.y &&
		this->centre.y  + this->size.y > bv->centre.y;	
#endif
}

bool AABB2D::IsIntersecting(const BoundingCircle* bv) const
{
	return bv->IsIntersecting(this);
}


