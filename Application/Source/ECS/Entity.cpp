#include "Entity.h"

namespace Nare
{
	EntityManager::EntityManager()
		: livingEntityCount_(0)
	{
		for (Entity entity = 0; entity < MaxEntities; ++entity)
		{
			availableEntities_.push(entity);
		}
	}

	Entity EntityManager::CreateEntity()
	{
		NR_CORE_ASSERT(livingEntityCount_ < MaxEntities, "Too many entities in existence.")

		Entity id = availableEntities_.front();
		availableEntities_.pop();
		++livingEntityCount_;

		return id;
	}

	void EntityManager::DestroyEntity(Entity entity)
	{
		NR_CORE_ASSERT(entity < MaxEntities, "Entity out of range.")

		// Invalidate the destroyed entity's signature
		signatures_[entity].reset();

		// Put the destroyed ID at the back of the queue
		availableEntities_.push(entity);
		--livingEntityCount_;
	}

	void EntityManager::SetSignature(Entity entity, Signature signature)
	{
		NR_CORE_ASSERT(entity < MaxEntities, "Entity out of range.")
		signatures_[entity] = signature;
	}

	Signature EntityManager::GetSignature(Entity entity)
	{
		NR_CORE_ASSERT(entity < MaxEntities, "Entity out of range.")

		return signatures_[entity];
	}
}
