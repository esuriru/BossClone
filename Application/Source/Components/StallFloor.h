#pragma once
#include "Component.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
class StallFloor : public Component
{
public:
	StallFloor(GameObject* go);

	void Update(double dt) override;

	void ResetBottlePosition();

	bool bottledown1;
	bool bottledown2;
	bool bottledown3;

	Collider* bottle1;
    Collider* bottle2;
	Collider* bottle3;

	Transform* bottleTransform1_;
	Transform* bottleTransform2_;
	Transform* bottleTransform3_;

	Collider* collider_;

private:
	
};

