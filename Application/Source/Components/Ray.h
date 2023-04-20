#pragma once
#include "Collider.h"
#include <glm/glm.hpp>
#include <vector>
#include "BoundingVolume.h"

class Ray : public Collider
{
public:
	Ray(GameObject* go);
	virtual ~Ray() = default;

	void Update(double dt) override;
	
	std::vector<Collider*> Raycast() const;

	void SetDirection(const glm::vec3& dir);
	const glm::vec3& GetDirection() const;
private:
	RayVolume* volume_;

};

