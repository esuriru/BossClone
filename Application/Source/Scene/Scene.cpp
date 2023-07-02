#include "Scene.h"
#include "ECS/Coordinator.h"

static Coordinator* s_Coordinator_ = Coordinator::Instance();

Scene::Scene()
{

}

void Scene::Start()
{
    // 'Initialize' every gameobject
    for (auto& gameObject : sceneObjects_)
    {
        gameObject->Start();
    }

    // Cache each renderer into a map.
    for (auto& gameObject : sceneObjects_)
    {
        auto& renderer = gameObject->GetRenderer();
        if (!renderer) continue;
        rendererMap_.insert(std::make_pair(renderer->GetSortingOrder(), renderer));
    }
}

void Scene::Update(Timestep ts)
{
    for (auto& gameObject : sceneObjects_)
    {
        gameObject->Update(ts);
    }
}

void Scene::FixedUpdate(float fixedStep)
{
    for (auto& gameObject : sceneObjects_)
    {
        gameObject->FixedUpdate(fixedStep);
    }
}

void Scene::Render()
{
    for (auto& rendererPair : rendererMap_)
    {
        rendererPair.second->Render();
    }
}
