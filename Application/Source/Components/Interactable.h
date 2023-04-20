#pragma once
#include "Component.h"
#include <cstdint>
#include <string>

class Ray;
class Interactable;

struct UseData
{
	Ray* useRay;
	uint32_t timesUsed = 0;
	Interactable* playerItemHolding;
};

class Interactable : public Component
{
public:
	Interactable(GameObject* go);
	
	virtual void Update(double dt) {};

	virtual void Use(UseData& data) {};

	// If 0, means unlimited.
	uint32_t timesUsable;
	std::string label;
private:

};

