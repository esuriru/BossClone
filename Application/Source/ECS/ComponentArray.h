#pragma once

#include "Core/Core.h"
#include <array>
#include "Entity.h"
#include "IComponentArray.h"
#include <unordered_map>
#include <cstdint>

template<typename T>
class ComponentArray : public IComponentArray
{
public:
	inline void InsertData(Entity entity, T component)
	{
		CC_ASSERT(entityToIndexMap_.find(entity) == entityToIndexMap_.end(), "Component added to the same entity more than once.");

		size_t newIndex = size_;
		entityToIndexMap_[entity] = newIndex;
		indexToEntityMap_[newIndex] = entity;
		componentArray_[newIndex] = component;

		++size_;
	}

	inline void RemoveData(Entity entity)
	{
		CC_ASSERT(entityToIndexMap_.find(entity) != entityToIndexMap_.end(), "Removing non-existent component.");

		// Copy the element at the end into the deleted element's place to remain density
		size_t indexOfRemovedEntity = entityToIndexMap_[entity];
		size_t indexOfLastElement = size_ - 1;
		componentArray_[indexOfRemovedEntity] = componentArray_[indexOfLastElement];

		Entity entityOfLastElement = indexToEntityMap_[indexOfLastElement];
		entityToIndexMap_[entityOfLastElement] = indexOfRemovedEntity;
		indexToEntityMap_[indexOfRemovedEntity] = entityOfLastElement;


		entityToIndexMap_.erase(entity);
		indexToEntityMap_.erase(indexOfLastElement);

		--size_;
	}

	inline T& GetData(Entity entity)
	{
		CC_ASSERT(entityToIndexMap_.find(entity) != entityToIndexMap_.end(), "Retrieving a component that does not exist.");

		return componentArray_[entityToIndexMap_[entity]];
	}

	inline void EntityDestroyed(Entity entity) override
	{
		if (entityToIndexMap_.find(entity) != entityToIndexMap_.end())
		{
			RemoveData(entity);
		}
	}

    inline auto GetEntities() -> std::vector<Entity>
    {
        std::vector<Entity> entities(entityToIndexMap_.size());
        for (const auto& kv_pair : entityToIndexMap_)
        {
            entities.emplace_back(kv_pair.first);
        }
        return entities;
    }

private:
	// Packed array of components set to MaxEntities
	std::array<T, MaxEntities> componentArray_;
	// std::array<T, 30> componentArray_;

	// Map from an entity ID to array index
	std::unordered_map<Entity, size_t> entityToIndexMap_;

	// Map from array index to entity ID
	std::unordered_map<size_t, Entity> indexToEntityMap_;

	size_t size_;
};
	