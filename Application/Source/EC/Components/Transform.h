#include "EC/Component.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

class Transform : public Component
{
public:
    Transform(Ref<GameObject> gameObject);

    const glm::vec3& GetPosition() const;
    const glm::quat& GetRotation() const;
    const glm::vec3& GetScale() const;

    glm::vec3 GetEulerAngles() const;

    void SetPosition(const glm::vec3& pos);
    void SetRotation(const glm::quat& rot);
    void SetScale(const glm::vec3& scl);

    const glm::mat4& GetWorldMatrix();

private:
    glm::mat4 localMatrix_;
    glm::mat4 worldMatrix_;

    glm::vec3 position_;
    glm::quat rotation_;
    glm::vec3 scale_;

    void CalculateWorldMatrix();

};