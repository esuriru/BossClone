#pragma once

#include "BoundingVolume2D.h"
#include <array>
#include <glm/glm.hpp>
#include "Collider2D.h"
#include "AABB2D.h"
#include "OBB2D.h"


// TODO - Change the bounding volume to a pointer to an AABB and OBB on the stack.
class BoxCollider2D : public Collider2D
{
public:
	BoxCollider2D(GameObject* go);
	void Update(double dt);
	void UpdateGravity() override;
	bool IsColliding(const Collider2D* c) const;
	//glm::vec2 GetSize(void) const;
	void Rotate(float clockwiseRotationDegrees) override;
	void Rotate(const glm::quat& rot) override;

	void SetPosition(const glm::vec2& pos) override;

protected:
	float localRotation_;

	void ComputeAxes();
	
private:
	bool rotated_;
	AABB2D normalBox_;
	OBB2D rotatedBox_;

	void UpdateRotation();
};

