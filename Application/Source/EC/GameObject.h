#pragma once

#include "Core/Core.h"
#include "Component.h"
#include <unordered_map>
#include <typeindex>

#include "Core/Timestep.h"

#include <glm/glm.hpp>

#include "Components/Transform.h"
#include "Components/Renderer.h"

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Component;
// NOTE - This game object will copy Unity's style in the essence that every game object will have a (cached) Transform component.

class GameObject : public std::enable_shared_from_this<GameObject>
{
private:
    template<typename T>
    using is_component = std::enable_if_t<std::is_base_of_v<Component, T>>;

public:
    GameObject();
    GameObject(const glm::vec3& position);
    GameObject(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

    void Start();
    void Update(Timestep ts);
    void FixedUpdate(float fixedStep);

    inline Transform& GetTransform()
    {
        return *transform_;
    }

    template<typename T, typename = is_component<T>>
    Ref<T> GetComponent()
    {
        auto iterator = components_.find(typeid(T));
        if (iterator != components_.end())
        {
            return std::static_pointer_cast<T>(iterator->second);
        }
        return nullptr;
    }

    template<typename T, typename = is_component<T>> 
    Ref<T> AddComponent() 
    {
        // TODO - Should the components have a weak pointer instead ?
        return std::static_pointer_cast<T>(components_.insert(
            std::make_pair(std::type_index(typeid(T)),
            CreateRef<T>(*this))).first->second);
    }

    template<typename T, typename = is_component<T>, typename... Args> 
    Ref<T> AddComponent(Args&&... args) 
    {
        // TODO - Should the components have a weak pointer instead ?
        return std::static_pointer_cast<T>(components_.insert(
            std::make_pair(std::type_index(typeid(T)),
            CreateRef<T>(*this, std::forward<Args>(args)...))).first->second);
    }

    template<typename T, typename = is_component<T>, typename... Args> 
    Ref<GameObject> SetComponent(Args&&... args) 
    {
        // TODO - Should the components have a weak pointer instead ?
        components_.insert(
            std::make_pair(std::type_index(typeid(T)),
            CreateRef<T>(*this, std::forward<Args>(args)...)));
        return shared_from_this();
    }


    template<typename T, typename = is_component<T>>
    void RemoveComponent()
    {
        components_.erase(typeid(T));
    }

    inline void SetActive(bool enabled)
    {
        enabled_ = enabled;
    }

    inline bool ActiveSelf() const
    {
        return enabled_;
    }

    inline void Render() 
    {
        renderer_->Render();
    }

    inline Renderer* GetRenderer()
    {
        return renderer_;
    }

private:
    std::unordered_map<std::type_index, Ref<Component>> components_;
    Transform* const transform_;
    Renderer* renderer_;

    bool enabled_;
    friend class Renderer;
     
};
