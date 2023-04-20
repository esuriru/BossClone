#pragma once
#include "Component.h"
#include "Interactable.h"
#include "Collider.h"

class InteractButton : public Component
{
public:
	InteractButton(GameObject* go);

	Interactable* Interact();

	void Bind(Interactable* i);

	void Update(double dt) override;
	void Reset();

private:
	bool isInteracting_;

	Collider* collider_;
	Interactable* interactable_;
};

