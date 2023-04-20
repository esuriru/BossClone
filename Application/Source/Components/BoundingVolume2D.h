#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Physics/Collision2D.h"

// Forward declarations for the visitor pattern
class OBB2D;
class AABB2D;
class BoundingCircle;
class Collider2D;

class BoundingVolume2D
{
public:
#pragma region Visitor_Pattern_Functions

#pragma region Intersections
	virtual bool IsIntersecting(const BoundingVolume2D* bv) const = 0;
	virtual bool IsIntersecting(const OBB2D* bv) const = 0;
	virtual bool IsIntersecting(const AABB2D* bv) const = 0;
	virtual bool IsIntersecting(const BoundingCircle* bv) const = 0;

#pragma endregion Intersections

#pragma endregion Visitor_Pattern_Functions

	BoundingVolume2D();
	BoundingVolume2D(Collider2D* attached, const glm::vec2& centre, const glm::vec2& size);
	
	virtual void Rotate(const glm::quat& rot);
	virtual void SetPosition(const glm::vec3& pos);
	virtual void SetPosition(const glm::vec2& pos);
	virtual void Reset() {};

	glm::vec2 centre;
	glm::vec2 size;	
	glm::quat orientation;
	bool isDirty;

	Collider2D* GetAttachedCollider() const;
protected:
	Collider2D* attachedToCollider_;

	virtual void AttachCollision(Collision2D& col) const;
};

