#pragma once
#include <vector>
#include "ECS/Entity.h"

class Camera;

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;

protected:
    std::vector<Entity> entities_;

};

