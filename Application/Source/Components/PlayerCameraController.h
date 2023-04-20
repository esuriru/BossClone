#pragma once
#include "CameraController.h"
#include <glm/glm.hpp>

class PlayerCameraController : public CameraController
{
public:
	PlayerCameraController(GameObject* go);

	void Follow(Transform* t);

private:
	void UpdateCamera() override;

	Transform* targetTransform_;

};

