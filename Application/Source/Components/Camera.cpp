#include "Camera.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "Core/Application.h"
#include "Scene/GameObject.h"
#include <glm/glm.hpp>
#include "Transform.h"
#include "MyMath.h"
#include <glm/gtc/matrix_transform.hpp>
#include "KeyboardController.h"
#include "Utils/Input.h"

static bool s_firstMouseUsage = true;

//KeyboardController* kc = KeyboardController::GetInstance();

static Input* input = Input::GetInstance();

Camera::Camera(GameObject* go)
	: Component(go)
	, targetTransform_(go->GetTransform())
{
	Camera::Reset();
}

void Camera::CalculateViewMatrix()
{
#if 0
	const auto& centre = _targetTransform->GetPosition();
	_right = glm::normalize(glm::cross(_front, { 0 ,1, 0 }));
	_up = glm::normalize(glm::cross(_right, _front));
	_target = centre + _front;
#endif

	//std::cout << "View matrix calculated\n";
	view_ = glm::inverse(targetTransform_->GetTransformMatrix());
}


const glm::mat4& Camera::GetViewMatrix()
{
	if (isDirty_)
	{
		CalculateViewMatrix();
		isDirty_ = false;
	}
	//std::cout << glm::to_string(_view) << "\n";
	return view_;
}

void Camera::Init(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
{
	targetTransform_->SetPosition(pos);
	targetTransform_->SetRotation(glm::quatLookAt(glm::normalize(target - pos), up));
	isDirty_ = true;
}

void Camera::Reset()
{
	targetTransform_->SetPosition({ 1, 0, 0 });
	targetTransform_->SetRotation(glm::quatLookAt(glm::vec3(0, 0, -1), glm::vec3( 0, 1, 0 )));
	isDirty_ = true;
}

void Camera::Update(double dt)
{
	//_targetTransform->SetRotation(glm::quatLookAt(_front, _up));
#if 0
	
	constexpr float CAMERA_SPEED = 10.f;

	if (input->IsKeyDown('I'))
		_targetTransform->Translate(this->_front * CAMERA_SPEED * static_cast<float>(dt)); //move camera up
	if (input->IsKeyDown('J'))
		_targetTransform->Translate(this->_right * -CAMERA_SPEED * static_cast<float>(dt)); //move camera left
	if (input->IsKeyDown('K'))
		_targetTransform->Translate(this->_front * -CAMERA_SPEED * static_cast<float>(dt)); //move camera down
	if (input->IsKeyDown('L'))
		_targetTransform->Translate(this->_right * CAMERA_SPEED * static_cast<float>(dt)); //move camera right
#endif

}

void Camera::UpdateViewMatrix(double mouseX, double mouseY)
{
#if 0
	const auto& mouseXf = static_cast<float>(mouseX);
	const auto& mouseYf = static_cast<float>(mouseY);
	if (s_firstMouseUsage)
	{
		_lastMouseCoords = { mouseXf, mouseYf };
		s_firstMouseUsage = false;
	}

	auto offset = glm::vec2(mouseXf, mouseYf) - _lastMouseCoords;
	_lastMouseCoords = { mouseXf, mouseYf };

	offset *= MOUSE_SENSITIVITY;

	// Pitch
	_cameraRotation.x -= offset.y;

	// Yaw
	_cameraRotation.y -= offset.x;

	if (_cameraRotation.y > 360)
		_cameraRotation.y -= 360;

	if (_cameraRotation.y < -360)
		_cameraRotation.y += 360;

	// Clamp it so that they aren't able to turn their heads upside down, or get gimbal lock.
	_cameraRotation.x = Math::Clamp(_cameraRotation.x, -89.0f, 89.0f);

	_targetTransform->SetEulerAngles(_cameraRotation);
	_isDirty = true;
#endif
}
