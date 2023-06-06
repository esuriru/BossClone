#pragma once

#include <bitset>
#include <set>
#include "ComponentManager.h"
#include "SystemManager.h"
#include "Entity.h"
#include "Utils/Singleton.h"
#include "Core/Window.h"
#include <memory>
#include <vector>

class Coordinator : public Utility::Singleton<Coordinator>
{
public:
    void Init();
    void Clear();

    Entity CreateEntity();

    void DestroyEntity(Entity entity);

#pragma region TEMPLATE_FUNCTIONS
    template<typename T>
    inline void RegisterComponent() const
    {
        componentManager_->RegisterComponent<T>();
    }

    template<typename T>
    inline void AddComponent(Entity entity, T component)
    {
        componentManager_->AddComponent<T>(entity, component);

        auto signature = entityManager_->GetSignature(entity);
        signature.set(componentManager_->GetComponentType<T>(), true);
        entityManager_->SetSignature(entity, signature);

        systemManager_->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    inline void RemoveComponent(Entity entity)
    {
        componentManager_->RemoveComponent<T>(entity);

        auto signature = entityManager_->GetSignature(entity);
        signature.set(componentManager_->GetComponentType<T>(), false);
        entityManager_->SetSignature(entity, signature);

        systemManager_->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    inline T& GetComponent(Entity entity)
    {
        return componentManager_->GetComponent<T>(entity);
    }

    template<typename T>
    inline ComponentType GetComponentType()
    {
        return componentManager_->GetComponentType<T>();
    }

    template<typename T>
    inline std::shared_ptr<T> RegisterSystem()
    {
        return systemManager_->RegisterSystem<T>();
    }

    template<typename T>
    inline void SetSystemSignature(Signature signature)
    {
        systemManager_->SetSignature<T>(signature);
    }

    template<typename T>
    inline auto View() -> std::vector<Entity>
    {
        return componentManager_->View<T>();
    }

    inline auto SetEventCallback(EventCallback eventCallback)
    {
        eventCallback_ = eventCallback;
    }

#pragma endregion TEMPLATE_FUNCTIONS
private:
    std::set<Entity> entities_;
    std::unique_ptr<EntityManager> entityManager_;
    std::unique_ptr<ComponentManager> componentManager_;
    std::unique_ptr<SystemManager> systemManager_;
    EventCallback eventCallback_;

};