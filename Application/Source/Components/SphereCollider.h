#pragma once
#include "Collider.h"

class BoundingSphere;

class SphereCollider : public Collider
{
public:
	SphereCollider(GameObject* go);

	void Update(double dt) override;
	void SetRadius(float radius);

private:
	BoundingSphere* vol_;
};

