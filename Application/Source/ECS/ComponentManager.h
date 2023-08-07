#pragma once

#include <memory>
#include "ComponentArray.h"
#include <unordered_map>

class ComponentManager
{
public:
    template <typename T>
    void RegisterComponent()
    {
        const char *typeName = typeid(T).name();

        CC_ASSERT(componentTypes_.find(typeName) == componentTypes_.end(), "Registering component type more than once.");

        // Add this type into the map
        componentTypes_.insert({typeName, nextComponentType_});

        // Add a new component array
        componentArrays_.insert({typeName, std::make_shared<ComponentArray<T>>()});

        ++nextComponentType_;
    }

    template <typename T>
    ComponentType GetComponentType()
    {
        const char *typeName = typeid(T).name();

        CC_ASSERT(componentTypes_.find(typeName) != componentTypes_.end(), "Component not registered before use");

        return componentTypes_[typeName];
    }

    template <typename T>
    void RemoveComponent(Entity entity)
    {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template <typename T>
    void AddComponent(Entity entity, T component)
    {
        GetComponentArray<T>()->InsertData(entity, component);
    }
    template <typename T>
    T &GetComponent(Entity entity)
    {
        return GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity)
    {
        for (const auto &pair : componentArrays_)
        {
            auto const &component = pair.second;

            component->EntityDestroyed(entity);
        }
    }

    template <typename T>
    auto View() -> std::vector<Entity>
    {
        return GetComponentArray<T>()->GetEntities();
    }


private:
    std::unordered_map<const char *, ComponentType> componentTypes_{};
    std::unordered_map<const char *, std::shared_ptr<IComponentArray>> componentArrays_{};

    ComponentType nextComponentType_{};

    template <typename T>
    inline std::shared_ptr<ComponentArray<T>> GetComponentArray()
    {
        const char *typeName = typeid(T).name();

        CC_ASSERT(componentTypes_.find(typeName) != componentTypes_.end(), "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(componentArrays_[typeName]);
    }
};