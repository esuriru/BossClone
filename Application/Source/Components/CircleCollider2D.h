#pragma once

#include "Collider2D.h"

class CircleCollider2D : public Collider2D
{
protected:
	float radius_;
	//bool IsColliding(const CircleCollider2D* c) const;
	//bool IsColliding(const BoxCollider2D* c) const;

public:
	void Update(double dt) override;
	float GetRadius(void) const;
	CircleCollider2D(GameObject* go);
	bool IsColliding(const Collider2D* c) const;

	void Rotate(float degrees) override;

};
	
