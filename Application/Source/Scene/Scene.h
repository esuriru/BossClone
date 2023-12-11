#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <functional>
#include <queue>

#include "EC/GameObject.h"

class Camera;
class OrthographicCamera;

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

    virtual void OnImGuiRender();

    void SetCamera(OrthographicCamera* camera);

    inline OrthographicCamera* GetCamera()
    {
        return camera_;
    }

    virtual Ref<GameObject> CreateGameObject();

    template<typename... Args>
    Ref<GameObject> CreateGameObject(Args&&... args)
    {
        Ref<GameObject> gameObject = CreateRef<GameObject>(std::forward<Args>(args)...);
        if (hasStarted_)
        {
            objectCreations_.push_back(gameObject);
            // objectCreationQueue_.push([&]()
            // {
            //     sceneObjects_.emplace_back(gameObject);
            //     gameObject->Start();
            // });
        }
        else
        {
            sceneObjects_.emplace_back(gameObject);
        }
        return gameObject;
    }

    virtual std::string GetName() = 0;

    Ref<GameObject> FindGameObjectByTag(const std::string& tag);

    void UpdateRenderer(Renderer* renderer);
    void FlushCreationQueue();
protected:
    std::vector<Ref<GameObject>> sceneObjects_;
    std::multimap<int, Renderer*, std::less<int>> rendererMap_;
    std::string name_;
    std::queue<std::function<void()>> objectCreationQueue_;
    std::vector<Ref<GameObject>> objectCreations_;

    OrthographicCamera* camera_;
    bool hasStarted_;
};

