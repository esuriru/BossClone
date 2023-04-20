#include "PanningCameraController.h"
#include "Camera.h"
//#include "KeyboardController.h"
#include "Utils/Input.h"
#include "Scene/GameObject.h"
#include "Transform.h"

PanningCameraController::PanningCameraController(GameObject* go)
	: CameraController(go)
{

}


void PanningCameraController::SetToLookAt(Transform* t)
{
	following_ = t;
}

void PanningCameraController::UpdateCamera()
{
	if (!following_)
		return;
#if 1
	static constexpr float _cameraSpeed = 10.f;

	//KeyboardController* input = KeyboardController::GetInstance();
	
	static Input* input = Input::GetInstance();

	if (input->IsKeyDown('I'))
		panningCoords_ += glm::vec2(deltaTime_ * _cameraSpeed, 0);
	if (input->IsKeyDown('J'))
		panningCoords_ += glm::vec2(0, deltaTime_ * -_cameraSpeed);
	if (input->IsKeyDown('K'))
		panningCoords_ += glm::vec2(deltaTime_ * -_cameraSpeed, 0);
	if (input->IsKeyDown('L'))
		panningCoords_ += glm::vec2(0, deltaTime_ * _cameraSpeed);

	float x = cosf(glm::radians(panningCoords_.x)) * cosf(glm::radians(this->panningCoords_.y));
	float y = sinf(glm::radians(panningCoords_.x));
	float z = cosf(glm::radians(panningCoords_.x)) * sinf(glm::radians(this->panningCoords_.y));

	cameraTransform_->SetPosition(glm::vec3(x, y, z) + following_->GetPosition());

	glm::vec3 dir = glm::normalize(cameraTransform_->GetPosition() - following_->GetPosition());
	glm::vec3 right = glm::normalize(glm::cross(dir, { 0, 1, 0 }));
	glm::vec3 up = glm::normalize(glm::cross(right, dir));
#endif

	//_cameraTransform->SetPosition({ 0, 0, 0 });

	//_cameraTransform->SetRotation(glm::quatLookAt(glm::normalize(glm::vec3( x, y, z)), { 0, 1, 0 }));
	//_camera->SetDirty(true);
	camera_->view_ = glm::lookAt(cameraTransform_->GetPosition(), following_->GetPosition(), up);
}
