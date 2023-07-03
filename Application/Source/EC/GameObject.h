#pragma once

#include "Core/Core.h"
#include "Component.h"
#include <unordered_map>
#include <typeindex>

#include "Core/Timestep.h"

#include "Components/Transform.h"
#include "Components/Renderer.h"

#include <memory>

class Component;
// NOTE - This game object will copy Unity's style in the essence that every game object will have a (cached) Transform component.

class GameObject : public std::enable_shared_from_this<GameObject>
{
private:
    template<typename T>
    using is_component = std::enable_if_t<std::is_base_of_v<Component, T>>;

public:
    GameObject();

    void Start();
    void Update(Timestep ts);
    void FixedUpdate(float fixedStep);

    inline Ref<Transform>& GetTransform()
    {
        return transform_;
    }

    template<typename T, typename = is_component<T>>
    Ref<T> GetComponent()
    {
        auto iterator = components_.find(typeid(T));
        if (iterator != components_.end())
        {
            return static_cast<Ref<T>>(iterator->second);
        }
        return nullptr;
    }

    template<typename T, typename = is_component<T>> 
    Ref<T> AddComponent() 
    {
        // TODO - Should the components have a weak pointer instead ?
        return std::static_pointer_cast<T>(components_.insert(
            std::make_pair(std::type_index(typeid(T)), CreateRef<T>(
            shared_from_this()))).first->second);
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

    inline void SetRenderer(Ref<Renderer> renderer)
    {
        renderer_ = renderer;
    }

    inline Ref<Renderer>& GetRenderer()
    {
        return renderer_;
    }

private:
    std::unordered_map<std::type_index, Ref<Component>> components_;
    Ref<Transform> transform_;
    Ref<Renderer> renderer_;

    bool enabled_;
     
};
