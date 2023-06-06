#include "Coordinator.h"
#include "Events/ApplicationEvent.h"

void Coordinator::Init()
{
    componentManager_ = std::make_unique<ComponentManager>();
    entityManager_ = std::make_unique<EntityManager>();
    systemManager_ = std::make_unique<SystemManager>();
}

void Coordinator::Clear()
{
    for (auto& e : entities_)
    {
        entityManager_->DestroyEntity(e);
        componentManager_->EntityDestroyed(e);
        systemManager_->EntityDestroyed(e);
    }
    entities_.clear();
}

Entity Coordinator::CreateEntity()
{
    Entity newEntity = entityManager_->CreateEntity();
    entities_.insert(newEntity);
    return newEntity;
}

void Coordinator::DestroyEntity(Entity entity)
{
    if (eventCallback_) 
    {
        OnEntityDestroyedEvent event(entity);
        eventCallback_(event);
    }

    entityManager_->DestroyEntity(entity);
    componentManager_->EntityDestroyed(entity);
    systemManager_->EntityDestroyed(entity);
    entities_.erase(entity);
}
