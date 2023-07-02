#pragma once

#include <vector>
#include <map>

#include "ECS/Entity.h"
#include "EC/GameObject.h"

class Camera;

class Scene
{
public:
	Scene();
	virtual ~Scene() {}

    /// @brief This function is run before the first frame update.
    virtual void Start();

    virtual void Update(Timestep ts);
    virtual void FixedUpdate(float fixedStep);

    virtual void Render();

protected:
    std::vector<Ref<GameObject>> sceneObjects_;

    std::multimap<int, Ref<Renderer>, std::greater<int>> rendererMap_;
};

