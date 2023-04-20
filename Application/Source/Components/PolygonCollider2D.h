#pragma once
#include "Collider2D.h"

class OBB2D;

class PolygonCollider2D : public Collider2D
{
public:
	PolygonCollider2D(GameObject* go);
	void Update(double dt) override;

	void SetToTriangleCollider();

	void Rotate(float clockwiseRotationDegrees) override;
protected:
	unsigned int numberOfVertices_;
	float localRotation_;
	OBB2D* bounds_;
};

