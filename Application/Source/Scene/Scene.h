#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <functional>

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

    virtual void OnDestroy();

    virtual Ref<GameObject> CreateGameObject();

    template<typename... Args>
    Ref<GameObject> CreateGameObject(Args&&... args)
    {
        Ref<GameObject> gameObject = CreateRef<GameObject>(std::forward<Args>(args)...);
        sceneObjects_.emplace_back(gameObject);
        return gameObject;
    }

    virtual std::string GetName() = 0;

    Ref<GameObject> FindGameObjectByTag(const std::string& tag);

protected:
    std::vector<Ref<GameObject>> sceneObjects_;
    std::multimap<int, Renderer*, std::less<int>> rendererMap_;
    std::string name_;
};

