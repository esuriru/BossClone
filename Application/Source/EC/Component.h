#include "Core/Core.h"

class GameObject;
class Component
{
public:
    virtual ~Component() = default;
    Component(const Component& other) = delete;

public:
    Ref<GameObject> gameObject;

protected:
    Component() = default;

};