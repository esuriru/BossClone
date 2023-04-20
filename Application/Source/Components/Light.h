#pragma once
#include <iostream>
#include <glm/glm.hpp>

#include "Component.h"

enum LightType
{
	Point, Directional, Spot
};

struct LightData
{
	glm::vec4 centre;
	glm::vec4 direction;
	glm::vec4 colour;

	LightType type;

	float power;

	float constant;
	float linear;
	float quadratic;

	float cos_cutoff;
	float cos_inner;

	float exponent;

};

class Light : public Component
{
public:
	Light(GameObject* go);
	~Light() override;

	virtual void Update(double dt) override;

	LightData* data;
};

