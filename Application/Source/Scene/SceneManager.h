#pragma once

#include "Utils/Singleton.h"
#include "Core/Core.h"
#include "Scene.h"

#include <string>

class SceneManager : public Utility::Singleton<SceneManager>
{
public:
    SceneManager() : activeScene_(nullptr) {}

    Ref<Scene> GetScene(const std::string& name);
    Ref<Scene> AddScene(const std::string& name, Ref<Scene> scene);
    void RemoveScene(const std::string& name);

    template<typename T>
    Ref<T> CreateScene()
    {
        auto scene = CreateRef<T>();
        sceneMap_[scene->GetName()] = scene;
        return scene;
    }

    inline Ref<Scene> GetActiveScene()
    {
        return activeScene_;
    }

    inline void SetActiveScene(Ref<Scene> scene, bool start = false)
    {
        activeScene_ = scene;
        if (start)
        {
            activeScene_->Start();
        }
    }

    inline void RefreshActiveScene()
    {
        activeScene_->OnDestroy();
        activeScene_->Start();
    }

private:
    std::unordered_map<std::string, Ref<Scene>> sceneMap_;
    Ref<Scene> activeScene_;

};
