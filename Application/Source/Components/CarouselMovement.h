#pragma once
#include "Component.h"
#include "Transform.h"

class CarouselMovement : public Component
{
public:
	CarouselMovement(GameObject* go);

	void Update(double dt);

private:
	Transform* targetTransform_;
};

