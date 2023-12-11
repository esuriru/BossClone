#pragma once

#include "Component.h"

#include "Core/Timestep.h"
#include "Core/Core.h"

#include "Components/Transform.h"
#include "Components/Renderer.h"

#include <unordered_map>
#include <typeindex>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using std::string;

class Collider2D;
// NOTE - This game object will copy Unity's style in the essence that every game object will have a (cached) Transform component.

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    template<typename T>
    using is_component = std::enable_if_t<std::is_base_of_v<Component, T>>;

public:
    GameObject();
    GameObject(const glm::vec3& position);
    GameObject(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

    void Start();
    void Update(Timestep ts);
    void FixedUpdate(float fixedStep);
    void OnDestroy();
    void OnTriggerEnter2D(Collider2D* other);
    void OnImGuiRender();
    void Message(string message);

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

    inline void SetTag(string tag)
    {
        tag_ = tag;
    }

    inline string GetTag()
    {
        return tag_;
    }

    inline bool CompareTag(const string& rhs)
    {
        return tag_ == rhs;
    }

private:
    std::unordered_map<std::type_index, Ref<Component>> components_;
    Ref<Transform> transformInternal_;
    Transform* const transform_;
    Renderer* renderer_;

    bool enabled_;
    string tag_;

    friend class Renderer;
     
};
