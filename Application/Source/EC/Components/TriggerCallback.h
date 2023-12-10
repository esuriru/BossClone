#pragma once

#include "EC/Component.h"

#include <functional>
#include <string>

class TriggerCallback : public Component
{
public:
    enum class Type
    {
        Tag
    };

    TriggerCallback(GameObject& gameObject);

    void OnTriggerEnter2D(Collider2D* other) override;

    inline void SetCallback(std::function<void(Collider2D*)> callback)
    {
        callback_ = callback;
    }

    inline void SetType(Type type)
    {
        type_ = type;
    }

    inline void SetTagCondition(std::string tag)
    {
        tag_ = tag;
    }


private:
    std::function<void(Collider2D*)> callback_ = nullptr;
    Type type_ = Type::Tag;
    std::string tag_;
};
