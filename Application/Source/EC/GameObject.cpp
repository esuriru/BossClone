#include "GameObject.h"

GameObject::GameObject()
{
    transform_ = AddComponent<Transform>();
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
