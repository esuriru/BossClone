#pragma once
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include "BoundingVolume2D.h"
#include "Projection.h"

class OBB2D : public BoundingVolume2D
{
public:
	bool IsIntersecting(const BoundingVolume2D* bv) const;

	OBB2D();
	OBB2D(Collider2D* attached,const glm::vec2& centre, const glm::vec2& size);
	OBB2D(Collider2D* attached, const std::vector<glm::vec2>& vertices);
	explicit OBB2D(const AABB2D* to_convert);

	virtual ~OBB2D() = default;

	float previousRotation;

	void RotateVertices(float clockwiseRotationDegrees);
	void Rotate(const glm::quat& rot);

	void Reset();

	std::vector<glm::vec2> originalVertices;
	std::vector<glm::vec2> originalAxes;

	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> axes;
#pragma region Factory_Functions
	static OBB2D* CreateRectangleOBB(Collider2D* attached, const glm::vec2& centre, const glm::vec2& size);
	static OBB2D* CreateTriangleOBB(Collider2D* attached, const glm::vec2& centre, const glm::vec2& size);


#pragma endregion Factory_Functions

#pragma region Intersections
	bool IsIntersecting(const OBB2D* bv) const;
	bool IsIntersecting(const AABB2D* bv) const;
	bool IsIntersecting(const BoundingCircle* bv) const;
	
#pragma endregion Intersections

	void SetPosition(const glm::vec2& pos) override;

	void SetOrientation(float degrees);
	void SetPosition(const glm::vec3& pos) override;
	Projection ProjectVertices(const glm::vec2& axis) const;

private:
#pragma region SAT
	std::pair<float, glm::vec2> PointSegmentDistance(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) const;
	struct Axis
	{
		glm::vec2 axis_direction;
		float axis_overlap{};
	};

	Axis FindMinOverlapAxis(const OBB2D* bv) const;

	mutable float tMin_;
	mutable float tMax_;

	mutable Axis minSepAxis_;
#pragma endregion SAT Separated axis theorem functions and variables
};

