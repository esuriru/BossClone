#pragma once

#include <glm/glm.hpp>
#include "Component.h"

class Transform;

class Camera : public Component
{
public:
	Camera(GameObject* go);

	virtual const glm::mat4& GetViewMatrix();

	virtual void Init(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);
	virtual void Reset();

	virtual void Update(double dt);

	virtual void UpdateViewMatrix(double mouseX, double mouseY);

	glm::mat4 projection;

protected:
	friend class PanningCameraController;
	friend class CameraController;

	glm::vec3 target_;
	glm::vec3 up_;
	glm::vec3 front_;
	glm::vec3 right_;
	glm::vec3 direction_;

	Transform* targetTransform_;

	void CalculateViewMatrix();

	glm::mat4 view_;

	Camera();
	virtual ~Camera() = default;

	glm::vec2 lastMouseCoords_;
	glm::vec3 cameraRotation_{}; // PITCH, YAW, ROLL
};



