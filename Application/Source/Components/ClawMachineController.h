#pragma once
#include "Component.h"

class GameObject;
class Transform;

class ClawMachineController : public Component
{
public:

	ClawMachineController(GameObject* go);

	void Update(double dt);
	void HandleKeyPress();

private:

	Transform* targetTransform_;
	Transform* targetChildTransform1_;
	Transform* targetChildTransform2_;
	Transform* targetChildTransform3_;

	float deltaTime_;
	bool down;
	bool up;
};

