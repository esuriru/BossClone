#include "SceneManager.h"

Ref<Scene> SceneManager::GetScene(const std::string &name)
{
    auto it = sceneMap_.find(name);
    return it == sceneMap_.end() ? nullptr : it->second;
}

Ref<Scene> SceneManager::AddScene(const std::string &name, Ref<Scene> scene)
{
    sceneMap_[name] = scene;
    return scene;
}

void SceneManager::RemoveScene(const std::string &name)
{
    sceneMap_.erase(name);
}
