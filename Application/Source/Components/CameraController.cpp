#include "CameraController.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "MyMath.h"
#include "KeyboardController.h"
#include "Scene/GameObject.h"
#include "Camera.h"
#include "Utils/Input.h"
#include "Core/Log.h"


CameraController::CameraController(GameObject* go)
	: Component(go)
	, camera_(gameObject_->GetComponent<Camera>())
	, cameraTransform_(camera_->GetGameObject()->GetTransform())
	, cameraSpeed_(35.f)
{

}

void CameraController::SetSpeed(float speed)
{
	cameraSpeed_ = speed;
}

void CameraController::Update(double dt)
{
	if (isDirty_)
	{
		camera_ = gameObject_->GetComponent<Camera>();
		if (camera_)
			cameraTransform_ = camera_->GetGameObject()->GetTransform();
		isDirty_ = false;
	}

	if (!camera_)
		return;

	deltaTime_ = static_cast<float>(dt);

	if (camera_->IsActive())
		UpdateCamera();
}

void CameraController::UpdateCamera()
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

	// Clamp it so that they aren't able to turn their heads upside down, or get gimbal lock.
	rotation.x = Math::Clamp(rotation.x, -89.0f, 89.0f);

	cameraTransform_->SetRotation(glm::quat(glm::radians(rotation)));
	//std::cout << glm::to_string(rotation)<< "\n";

	constexpr glm::vec3 BACK = { 0, 0, -1 };
	constexpr glm::vec3 RIGHT = { 1, 0, 0 };

	const auto& front = cameraTransform_->GetRotation() * BACK;
	const auto& right = cameraTransform_->GetRotation() * RIGHT;

	if (input->IsKeyDown('I'))
		cameraTransform_->Translate(front * cameraSpeed_ * deltaTime_); //move camera up
	if (input->IsKeyDown('J'))
		cameraTransform_->Translate(right * -cameraSpeed_ * deltaTime_); //move camera left
	if (input->IsKeyDown('K'))
		cameraTransform_->Translate(front * -cameraSpeed_ * deltaTime_); //move camera down
	if (input->IsKeyDown('L'))
		cameraTransform_->Translate(right * cameraSpeed_ * deltaTime_); //move camera right

	camera_->isDirty_ = true;

	//CC_INFO(glm::to_string(front));
}
