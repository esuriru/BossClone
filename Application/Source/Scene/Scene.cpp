#include "Scene.h"
#include "ECS/Coordinator.h"
#include "SceneManager.h"

static Coordinator* s_Coordinator_ = Coordinator::Instance();

Scene::Scene(const std::string &name)
    : name_(name)
{
    // SceneManager::Instance()->AddScene(name, shared_from_this());
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
        auto renderer = gameObject->GetRenderer();
        if (!renderer) continue;
        rendererMap_.insert(std::make_pair(renderer->GetSortingOrder(), renderer));
    }
}

void Scene::Update(Timestep ts)
{
    for (auto& gameObject : sceneObjects_)
    {
        if (gameObject->ActiveSelf())
        {
            gameObject->Update(ts);
        }
    }
}

void Scene::FixedUpdate(float fixedStep)
{
    for (auto& gameObject : sceneObjects_)
    {
        if (gameObject->ActiveSelf())
        {
            gameObject->FixedUpdate(fixedStep);
        }
    }
}

void Scene::Render()
{
    for (auto& rendererPair : rendererMap_)
    {
        if (rendererPair.second->enabled && 
            rendererPair.second->GetGameObject().ActiveSelf())
        {
            rendererPair.second->Render();
        }
    }
}

void Scene::OnDestroy()
{
    for (auto& gameObject : sceneObjects_)
    {
        if (gameObject->ActiveSelf())
        {
            gameObject->OnDestroy();
        }
    }
}

Ref<GameObject> Scene::CreateGameObject()
{
    Ref<GameObject> gameObject = CreateRef<GameObject>();
    sceneObjects_.emplace_back(gameObject);
    return gameObject;
}

Ref<GameObject> Scene::FindGameObjectByTag(const std::string & tag)
{
    // TODO - Cache the game objects by tags
    for (auto& gameObject : sceneObjects_)
    {
        if (gameObject->CompareTag(tag))
        {
            return gameObject;
        }
    }
    return nullptr;
}


