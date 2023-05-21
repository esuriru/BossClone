#include "Coordinator.h"
#include "Events/ApplicationEvent.h"

void Coordinator::Init()
{
    componentManager_ = std::make_unique<ComponentManager>();
    entityManager_ = std::make_unique<EntityManager>();
    systemManager_ = std::make_unique<SystemManager>();
}

Entity Coordinator::CreateEntity()
{
    return entityManager_->CreateEntity();
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

}
