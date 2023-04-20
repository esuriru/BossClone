#pragma once
#include "Component.h"
#include "Transform.h"

class DebugMovement : public Component
{
public:
	DebugMovement(GameObject* go);

	void Update(double dt);

private:
	Transform* targetTransform_;
};

