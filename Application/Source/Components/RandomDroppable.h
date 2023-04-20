#pragma once
#include "Droppable.h"

class RandomDroppable : public Droppable
{
public:
	RandomDroppable(GameObject* go);

	void UpdateGravity() override;
	void FixedUpdate() override;

	float lowerLimit;
	float upperLimit;
private:
};

