#pragma once
#include <utility>

#include "BoundingVolume2D.h"
#include "Projection.h"


struct Collision2D;


class BoundingCircle : public BoundingVolume2D
{
public:
	bool IsIntersecting(const BoundingVolume2D* bv) const;

	float radius;
	bool IsIntersecting(const OBB2D* bv) const;
	bool IsIntersecting(const AABB2D* bv) const;
	bool IsIntersecting(const BoundingCircle* bv) const;

	BoundingCircle();
	BoundingCircle(Collider2D* attached,const glm::vec2& centre,float radius);

	virtual ~BoundingCircle() = default;

private:
	Projection ProjectVertices(const glm::vec2& axis) const;

	std::pair<bool, Collision2D> IsIntersecting(const glm::vec2& start, const glm::vec2& end) const ;
	std::pair<bool, Collision2D> CircleToPoint(const glm::vec2& point) const;

};

