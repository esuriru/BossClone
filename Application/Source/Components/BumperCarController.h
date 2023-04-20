#pragma once
#include "Component.h"
#include "RigidBody.h"

class Transform;
class GameObject;
class Camera;
class Interactable;
class InteractButton;

class BumperCarController : public Component
{
public:
	BumperCarController(GameObject* go);
	virtual void Update(double dt) override;

	virtual void HandleKeyPress();

	void BindCamera(Camera* cam);
	void CameraFollowTransform(Transform* other);
	void SetCameraPositionOffset(const glm::vec3& offset);

private:
	Transform* targetTransform_;
	RigidBody* rigidBody_;

	glm::vec3 cameraOffset_{};
	glm::vec3 rotation{};
	Transform* followingCameraTransform_;
	Camera* refCamera_;

	float rotation_velocity;

	float deltaTime_;

};

