#include "GameObject.h"

GameObject::GameObject()
    : transform_(AddComponent<Transform>().get())
    , renderer_(nullptr)
    , enabled_(true)
{
    // transform_ = AddComponent<Transform>().get();
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
