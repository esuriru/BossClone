#pragma once
#include "MeshRenderer.h"

class UIRenderer : public MeshRenderer
{
public:
	UIRenderer(GameObject* go);

	void Render() override;

	bool billboarded;
};

