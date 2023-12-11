#pragma once
#include "Core/Core.h"
#include "Core/Timestep.h"

#include <string>

class GameObject;
class Transform;
class Collider2D;

class Component
{
public:
    template<typename T>
    using is_component = std::enable_if_t<std::is_base_of_v<Component, T>>;

    virtual ~Component() = default;
    Component(GameObject& gameObject) : gameObject_(gameObject) {}

    Component(const Component& other) = delete;
    Component& operator=(const Component& other) = delete;

    virtual void Start() {}
    
    virtual void Update(Timestep ts) {}

    // TODO - Should this be the implementation?
    virtual void FixedUpdate(float fixedStep) {}

    virtual void OnDestroy() {}
    virtual void OnTriggerEnter2D(Collider2D* other) {}

    virtual void Message(std::string message) {}
    virtual void OnImGuiRender() {}

    inline GameObject& GetGameObject()
    {
        return gameObject_;
    }

    Transform& GetTransform();

public:
    bool enabled;
    bool isDirty;

protected:
    Component() = default;
    GameObject& gameObject_;

    Ref<GameObject> FindGameObjectByTag(const std::string& tag);

};
