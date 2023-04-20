#pragma once
#include "CameraController.h"
#include "Transform.h"
#include <glm/glm.hpp>

class PanningCameraController : public CameraController
{
public:
	PanningCameraController(GameObject* go);

	void SetToLookAt(Transform* t);
private:
	void UpdateCamera() override;

	Transform* following_;
	glm::vec3 offset_;
	glm::vec3 target_;
	glm::vec2 panningCoords_;
};

