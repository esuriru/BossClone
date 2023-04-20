#pragma once
#include "BoundingVolume2D.h"

class AABB2D : public BoundingVolume2D
{
public:
	bool IsIntersecting(const BoundingVolume2D* bv) const;

	AABB2D();
	AABB2D(Collider2D* attached, const glm::vec2& centre, const glm::vec2& size);
	virtual ~AABB2D() = default;
protected:
#pragma region Intersections
	bool IsIntersecting(const OBB2D* bv) const;
	bool IsIntersecting(const AABB2D* bv) const;
	bool IsIntersecting(const BoundingCircle* bv) const;
#pragma endregion Intersections


};

