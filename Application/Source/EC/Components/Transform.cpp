#include "Transform.h"

#include "glm/gtc/matrix_transform.hpp"
#include "Utils/Util.h"

Transform::Transform(GameObject& gameObject)
    : Component(gameObject)
    , position_()
    , rotation_(glm::identity<glm::quat>())
    , scale_(1.0f)
{
}

Transform::Transform(GameObject &gameObject, const glm::vec3 &position)
    : Component(gameObject)
    , position_(position)
    , rotation_(glm::identity<glm::quat>())
    , scale_(1.0f)
{
}

Transform::Transform(GameObject &gameObject, const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
    : Component(gameObject)
    , position_(position)
    , rotation_(rotation)
    , scale_(scale)
{
}

const glm::vec3 &Transform::GetPosition() const
{
    return position_;
}

const glm::quat &Transform::GetRotation() const
{
    return rotation_;
}

const glm::vec3 &Transform::GetScale() const
{
    return scale_;
}

glm::vec3 Transform::GetEulerAngles() const
{
    return glm::eulerAngles(rotation_);
}

void Transform::SetPosition(const glm::vec3 &pos)
{
    position_ = pos;
    isDirty = true;
}

void Transform::SetRotation(const glm::quat &rot)
{
    rotation_ = rot;
    isDirty = true;
}

void Transform::SetScale(const glm::vec3 &scl)
{
    scale_ = scl;
    isDirty = true;
}

void Transform::Translate(const glm::vec3 & trans)
{
    position_ += trans;
    isDirty = true;
}

const glm::mat4 &Transform::GetWorldMatrix() 
{
    InternalCheckDirty();
    return worldMatrix_;
}

void Transform::AddCallback(void *owner, std::function<void(Transform &)> callback)
{
    onChangeCallbacks_.insert({ owner, callback });
}

void Transform::RemoveCallback(void *owner)
{
    onChangeCallbacks_.erase(owner);
}

void Transform::InternalCheckDirty()
{
    if (isDirty)
    {
        isDirty = false;
        CalculateWorldMatrix();

        for (auto& callbackPair : onChangeCallbacks_)
        {
            callbackPair.second(*this);
        }
    }
}

void Transform::CalculateWorldMatrix()
{
    localMatrix_ = glm::translate(glm::mat4(1.0f), position_)
            * glm::mat4_cast(glm::quat(rotation_))
            * glm::scale(glm::mat4(1.0f), scale_);
    
    // TODO - For now, no parenting will be implemented.
    worldMatrix_ = localMatrix_;
}
