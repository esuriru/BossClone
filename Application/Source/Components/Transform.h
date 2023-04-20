#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/quaternion.hpp>

class Transform : public Component
{
private:
	glm::vec3 position_;
	glm::quat rotation_;
	glm::vec3 scale_;

	glm::mat4 worldMatrix_;
	glm::mat4 localMatrix_;

	Transform* parent_;
	std::vector<Transform*> children_;

	unsigned short childIndex_ = 0;

	void CalculateTransformMatrix();

	void ResetDirectionVectors();
	void UpdateDirectionVectors();

	void ChangeLocalToWorld(Transform* refWorld);
public:
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 forward;

#if 0
	enum RotationOrder
	{

	};
#endif

	Transform(GameObject* go);

	glm::vec3 GetFront();

	// Setting parent will make it dirty.
	void SetParent(Transform* parent);
	Transform* GetParent() const;

	Transform* GetChild(uint32_t index) const;

	void Translate(const glm::vec3& trans);
	void Rotate(const glm::quat& rot);

	void SetPosition(const glm::vec3& pos);
	void SetRotation(const glm::quat& rot);
	void SetEulerAngles(const glm::vec3& rot);
	void SetScale(const glm::vec3& scl);

	glm::vec3 InverseTransformPoint(const glm::vec3& point);
	
	glm::vec3 GetPosition(void) const;
	glm::vec3 GetEulerAngles(void) const;
	const glm::quat& GetRotation(void) const;
	glm::vec3 GetScale(void) const;

	glm::vec3 GetWorldPosition(void);
	glm::mat4 GetTransformMatrix();

	void ForceUpdate();

	void Follow(Transform* other);

	void Update(double dt);

};

