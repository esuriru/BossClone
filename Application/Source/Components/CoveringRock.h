#pragma once
#include "Component.h"
#include "Collider.h"

class CoveringRock : public Component
{
public:
	CoveringRock(GameObject* go);

	void Update(double dt) override;

	void BindObject(std::shared_ptr<GameObject> objectToHide);

private:
	Collider* targetCollider_;

	std::shared_ptr<GameObject> hidingObject_;
};

