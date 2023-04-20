#pragma once
#include "Component.h"


class RigidBody;
class PlayerMovement : public Component
{
public:
	PlayerMovement(GameObject* go);
	void Update(double dt) override;

private:
	bool isAirborne_;
	float deltaTime_;
	void HandleKeyPress();

	RigidBody* targetBody_;

};

