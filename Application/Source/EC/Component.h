#pragma once
#include "Core/Core.h"
#include "Core/Timestep.h"

class GameObject;
class Transform;

class Component
{
public:
    virtual ~Component() = default;
    Component(GameObject& gameObject) : gameObject_(gameObject) {}

    Component(const Component& other) = delete;
    Component& operator=(const Component& other) = delete;

    virtual void Start() {}
    
    virtual void Update(Timestep ts) {}

    // TODO - Should this be the implementation?
    virtual void FixedUpdate(float fixedStep) {}

    virtual void OnDestroy() {}

    inline GameObject& GetGameObject()
    {
        return gameObject_;
    }

    template<typename T>
    inline Ref<T> GetComponent()
    {
        return gameObject_->GetComponent<T>();
    }

    Transform& GetTransform();

public:
    bool enabled;
    bool isDirty;

protected:
    Component() = default;
    GameObject& gameObject_;

};