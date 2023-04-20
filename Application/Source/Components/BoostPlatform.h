#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class Collider2D;

class BoostPlatform : public Component
{
private:
	glm::vec3 forceToAddOn_;
	Collider2D* col_;
	Collider2D* platformBase_;
public:
	BoostPlatform(GameObject* go);

	void Update(double dt) override;
	void SetForce(const glm::vec3& force);
	void SetPlatformBase( Collider2D* platformBase);
};

