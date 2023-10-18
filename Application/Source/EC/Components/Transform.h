#pragma once

#include "EC/Component.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <vector>
#include <functional>
#include <unordered_map>

class Transform : public Component
{
public:
    Transform(GameObject& gameObject);
    Transform(GameObject& gameObject, const glm::vec3& position);
    Transform(GameObject& gameObject, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

    const glm::vec3& GetPosition() const;
    const glm::quat& GetRotation() const;
    const glm::vec3& GetScale() const;

    glm::vec3 GetEulerAngles() const;

    void SetPosition(const glm::vec3& pos);
    void SetRotation(const glm::quat& rot);
    void SetScale(const glm::vec3& scl);

    const glm::mat4& GetWorldMatrix();

    void AddCallback(void* owner, std::function<void(Transform&)> callback);
    void RemoveCallback(void* owner);

private:
    std::unordered_map<void*, std::function<void(Transform&)>> onChangeCallbacks_;

    void InternalCheckDirty();

    glm::mat4 localMatrix_;
    glm::mat4 worldMatrix_;

    glm::vec3 position_;
    glm::quat rotation_;
    glm::vec3 scale_;

    void CalculateWorldMatrix();
};