#pragma once
#include "Component.h"
#include "Light.h"
#include "Ray.h"

class SpotlightController : public Component
{
public:
	SpotlightController(GameObject* go);

	void Update(double dt) override;

	inline void BindRay(Ray* ray)
	{
		ray_ = ray;
	}

private:
	Light* attachedLight_;
	Ray* ray_;
};

