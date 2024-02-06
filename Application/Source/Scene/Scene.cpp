#include "Scene.h"
#include "ECS/Coordinator.h"
#include "SceneManager.h"

#include "Renderer/OrthographicCamera.h"

static Coordinator* s_Coordinator_ = Coordinator::Instance();

Scene::Scene(const std::string &name)
    : name_(name)
    , camera_(nullptr)
    , hasStarted_(false)
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

    hasStarted_ = true;
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

    sceneObjects_.clear();
    objectCreations_.clear();
    while (!objectCreationQueue_.empty())
    {
        objectCreationQueue_.pop();
    }
    rendererMap_.clear();
    hasStarted_ = false;
}

void Scene::OnImGuiRender()
{
    for (auto& gameObject : sceneObjects_)
    {
        if (gameObject->ActiveSelf())
        {
            gameObject->OnImGuiRender();
        }
    }
}

void Scene::SetCamera(OrthographicCamera* camera)
{
    camera_ = camera;
}

Ref<GameObject> Scene::CreateGameObject()
{
    Ref<GameObject> gameObject = CreateRef<GameObject>();
    if (hasStarted_)
    {
        objectCreations_.push_back(gameObject);
    }
    else
    {
        sceneObjects_.emplace_back(gameObject);
    }
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

void Scene::UpdateRenderer(Renderer *renderer)
{
    if (hasStarted_)
    {
        rendererMap_.insert(std::make_pair(renderer->GetSortingOrder(), renderer));
    }
}

void Scene::FlushCreationQueue()
{
    for (uint32_t i = 0; i < objectCreations_.size(); ++i)
    {
        sceneObjects_.push_back(objectCreations_[i]);
        objectCreations_[i]->Start();
    }
    objectCreations_.clear();
}
