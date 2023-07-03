#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>

#include "EC/GameObject.h"

class Camera;

class Scene : public std::enable_shared_from_this<Scene>
{
public:
	Scene(const std::string& name);
	virtual ~Scene() {}

    /// @brief This function is run before the first frame update.
    virtual void Start();

    virtual void Update(Timestep ts);
    virtual void FixedUpdate(float fixedStep);

    virtual void Render();

    virtual Ref<GameObject> CreateGameObject();
    virtual std::string GetName() = 0;

protected:
    std::vector<Ref<GameObject>> sceneObjects_;
    std::multimap<int, Ref<Renderer>, std::greater<int>> rendererMap_;
    std::string name_;
};

