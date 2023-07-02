#include "Transform.h"
#include "glm/gtc/matrix_transform.hpp"

Transform::Transform(Ref<GameObject> gameObject)
    : Component(gameObject)
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

const glm::mat4 &Transform::GetWorldMatrix() 
{
    if (isDirty)
    {
        isDirty = false;
        CalculateWorldMatrix();
    }
    return worldMatrix_;
}

void Transform::CalculateWorldMatrix()
{
    localMatrix_ = glm::translate(glm::mat4(1.0f), position_)
            * glm::mat4_cast(glm::quat(rotation_))
            * glm::scale(glm::mat4(1.0f), scale_);
    
    // TODO - For now, no parenting will be implemented.
    worldMatrix_ = localMatrix_;
}
