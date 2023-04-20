#pragma once
#include "Collider.h"
#include <vector>


class TerrainVolume;
class TerrainCollider : public Collider
{
public:
	TerrainCollider(GameObject* go);

	void Update(double dt) override;

	float FindHeight(float x, float z) const;

protected:
	friend class Terrain;
	
	TerrainVolume* volume_;
	
	glm::vec3 position_;
};


