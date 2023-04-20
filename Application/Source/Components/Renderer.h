#pragma once
#include "Component.h"
class Renderer : public Component
{
public:
	Renderer(GameObject* go);

	virtual ~Renderer() = default;
	virtual void Render() = 0;

	unsigned int renderOrder = 0;
};

