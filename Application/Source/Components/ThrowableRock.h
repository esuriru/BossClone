#pragma once
#include "Interactable.h"

class Transform;
class Collider;
class ThrowableRock : public Interactable
{
public:
	ThrowableRock(GameObject* go);
	virtual void HandleKeyPress();
	void Update(double dt) override;
	void Use(UseData& data) override;
	float force_scalar;
private:
	Transform* targetTransform_;
	Collider* collider_;
};

