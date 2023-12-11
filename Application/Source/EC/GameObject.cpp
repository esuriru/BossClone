#include "GameObject.h"

GameObject::GameObject()
    : transformInternal_(AddComponent<Transform>())
    , transform_(transformInternal_.get())
    , renderer_(nullptr)
    , enabled_(true)
{
    // transform_ = AddComponent<Transform>().get();
}

GameObject::GameObject(const glm::vec3 &position)
    : transformInternal_(AddComponent<Transform>(position))
    , transform_(transformInternal_.get())
    , renderer_(nullptr)
    , enabled_(true)
{

}

GameObject::GameObject(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
    : transformInternal_(AddComponent<Transform>(position, rotation, scale))
    , transform_(transformInternal_.get())
    , renderer_(nullptr)
    , enabled_(true)
{
}

void GameObject::Start()
{
    for (auto& component : components_)
    {
        if (component.second->enabled)
        {
            component.second->Start();
        }
    }
}

void GameObject::Update(Timestep ts)
{
    for (auto& component : components_)
    {
        if (component.second->enabled)
        {
            component.second->Update(ts);
        }
    }
}

void GameObject::FixedUpdate(float fixedStep)
{
    for (auto& component : components_)
    {
        if (component.second->enabled)
        {
            component.second->FixedUpdate(fixedStep);
        }
    }
}

void GameObject::OnDestroy()
{
    for (auto& component : components_)
    {
        if (component.second->enabled)
        {
            component.second->OnDestroy();
        }
    }
}

void GameObject::OnTriggerEnter2D(Collider2D *other)
{
    for (auto& component : components_)
    {
        if (component.second->enabled)
        {
            component.second->OnTriggerEnter2D(other);
        }
    }
}

void GameObject::OnImGuiRender()
{
    for (auto& component : components_)
    {
        if (component.second->enabled)
        {
            component.second->OnImGuiRender();
        }
    }
}

void GameObject::Message(string message)
{
    for (auto& component : components_)
    {
        if (component.second->enabled)
        {
            component.second->Message(message);
        }
    }
}
