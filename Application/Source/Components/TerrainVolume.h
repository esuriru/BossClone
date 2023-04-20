#pragma once
#include "BoundingVolume.h"
#include <vector>

class TerrainVolume : public BoundingVolume
{
public:
	TerrainVolume(Collider* parent);

	inline bool IsIntersecting(const BoundingVolume* other) const
	{
		return other->IsIntersecting(this);
	}

	bool IsIntersecting(const TerrainVolume* other) const override;
	bool IsIntersecting(const AABB* other) const override;
	bool IsIntersecting(const BoundingSphere* other) const override;
	bool IsIntersecting(const RayVolume* other) const override;

	std::pair<float, glm::vec3> GetHeightAndNormal(float x, float z) const;
	float GetHeight(float x, float z) const;

private:
	friend class Terrain;

	std::vector<std::vector<float>> heights_;
	std::vector<std::vector<glm::vec3>> normals_;
	float inverseHeightsRowMinusOne; // for calculation

};

