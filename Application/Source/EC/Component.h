#pragma once
#include "Core/Core.h"
#include "Core/Timestep.h"

class GameObject;
class Component
{
public:
    virtual ~Component() = default;
    Component(Ref<GameObject> gameObject) : gameObject_(gameObject) {}

    Component(const Component& other) = delete;
    Component& operator=(const Component& other) = delete;

    virtual void Start() {}
    
    virtual void Update(Timestep ts) {}

    // TODO - Should this be the implementation?
    virtual void FixedUpdate(float fixedStep) {}

    inline Ref<GameObject>& GetGameObject()
    {
        return gameObject_;
    }

public:
    bool enabled;
    bool isDirty;

protected:
    Component() = default;
    Ref<GameObject> gameObject_;

};