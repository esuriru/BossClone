#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class Transform;
class Camera;
class GameObject;

constexpr float MOUSE_SENSITIVITY = 0.10f;
class CameraController : public Component
{
public:
	CameraController(GameObject* go);

	void SetSpeed(float speed);
	virtual void Update(double dt) override;


protected:
	glm::vec3 rotation{  };
	Camera* camera_;
	Transform* cameraTransform_;

	float cameraSpeed_;
	float deltaTime_;

	virtual void UpdateCamera();
};

