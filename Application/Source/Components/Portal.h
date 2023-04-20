#pragma once
#include <string>
#include "Component.h"

class SphereCollider;

class Portal : public Component
{
public:
	Portal(GameObject* go);

	void Update(double dt);
	std::string sceneName = "NULL";
private:
	SphereCollider* col_;
};

