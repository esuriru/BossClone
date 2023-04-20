#pragma once
#include "Component.h"


class Transform;
class Collider2D;

class FallingObject : public Component
{
public:
	FallingObject(GameObject* go);

	float lifetime;
	void ResetPosition();
private:
	Transform* transform_;
	Collider2D* referenceCollider_;
	float objectRotation_;
	float rateOfSpin_;
	float timer_;


	void Update(double dt) override;
};

