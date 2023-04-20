#include "PlayerCameraController.h"
#include <glm/gtx/string_cast.hpp>
#include "KeyboardController.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "Scene/GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Utils/Input.h"
#include "MyMath.h"

PlayerCameraController::PlayerCameraController(GameObject* go)
	: CameraController(go)
	, targetTransform_(nullptr)
{

}

void PlayerCameraController::Follow(Transform* t)
{
	targetTransform_ = t;
}

void PlayerCameraController::UpdateCamera()
{
	if (targetTransform_ && camera_)
	{
		//static KeyboardController* keyboardInstance = KeyboardController::GetInstance();
		
		static Input* input = Input::GetInstance();

		auto offset = input->GetMouseOffset();

		offset *= MOUSE_SENSITIVITY;

		// Pitch
		rotation.x -= offset.y;

		// Yaw
		rotation.y -= offset.x;

		if (rotation.y > 360)
			rotation.y -= 360;

		if (rotation.y < -360)
			rotation.y += 360;

		// TODO - Might make it so that the player can't move their head all the way down.
		// Clamp it so that they aren't able to turn their heads upside down, or get gimbal lock.
		rotation.x = Math::Clamp(rotation.x, -89.0f, 89.0f);
		cameraTransform_->SetRotation(glm::quat(glm::radians(rotation)));
		//std::cout << glm::to_string(rotation)<< "\n";

		//_cameraTransform->Follow(_targetTransform);

		camera_->SetDirty(true);
	}
}
