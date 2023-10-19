#include "GameObject.h"

GameObject::GameObject()
    : transform_(AddComponent<Transform>().get())
    , renderer_(nullptr)
    , enabled_(true)
{
    // transform_ = AddComponent<Transform>().get();
}

GameObject::GameObject(const glm::vec3 &position)
    : transform_(AddComponent<Transform>(position).get())
    , renderer_(nullptr)
    , enabled_(true)
{

}

GameObject::GameObject(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
    : transform_(AddComponent<Transform>(position, rotation, scale).get())
    , renderer_(nullptr)
    , enabled_(true)
{
}

void GameObject::Start()
{
    for (auto& component : components_)
    {
        component.second->Start();
    }
}

void GameObject::Update(Timestep ts)
{
    for (auto& component : components_)
    {
        component.second->Update(ts);
    }
}

void GameObject::FixedUpdate(float fixedStep)
{
    for (auto& component : components_)
    {
        component.second->FixedUpdate(fixedStep);
    }
}

void GameObject::OnDestroy()
{
    for (auto& component : components_)
    {
        component.second->OnDestroy();
    }
}

void GameObject::OnTriggerEnter2D(Collider2D *other)
{
    for (auto& component : components_)
    {
        component.second->OnTriggerEnter2D(other);
    }
}
