#pragma once
#include "Component.h"

class Collider;

class FinishPoint : public Component
{
public:
	FinishPoint(GameObject* go);

	void Update(double dt) override;

	void BindObject(GameObject* obj);
	void BindConfetti(GameObject* con);
private:
	Collider* target_;
	GameObject* bindedObject_;
	GameObject* confetti_;

	bool hasWon_;
	bool redIn_;
	bool greenIn_;
	bool blueIn_;
};

