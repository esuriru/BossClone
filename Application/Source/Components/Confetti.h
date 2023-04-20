#pragma once 
#include "Component.h"

class Confetti : public Component
{
public:
	Confetti(GameObject* go);

	void Update(double dt);
private:
};