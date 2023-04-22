#pragma once
#include <vector>
#include "ECS/Entity.h"

class Camera;

class Scene
{
public:
	Scene();
	virtual ~Scene() {}

    auto CreateEntity() -> Entity;

protected:
    std::vector<Entity> entities_;

};

