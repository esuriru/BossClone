#include "Coordinator.h"

namespace Nare
{
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
		entityManager_->DestroyEntity(entity);

		componentManager_->EntityDestroyed(entity);

		systemManager_->EntityDestroyed(entity);
	}
}
