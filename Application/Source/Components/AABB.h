#pragma once
#include "BoundingVolume.h"

class AABB : public BoundingVolume
{
public:
	AABB(Collider* parent, glm::vec3 centre, glm::vec3 size);

	inline bool IsIntersecting(const BoundingVolume* other) const
	{
		return other->IsIntersecting(this);
	}

	bool IsIntersecting(const TerrainVolume* other) const override;
	bool IsIntersecting(const AABB* other) const override;
	bool IsIntersecting(const BoundingSphere* other) const override;
	bool IsIntersecting(const RayVolume* other) const override;

	void SetPosition(const glm::vec3& pos) override;
	
	void SetSize(const glm::vec3& size);

	glm::vec3 size;
private:
	glm::vec3 min_;
	glm::vec3 max_;
};

