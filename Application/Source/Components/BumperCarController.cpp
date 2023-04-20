#include "BumperCarController.h"
#include "RigidBody.h"
#include "MyMath.h"
#include <glm/gtc/quaternion.hpp>
#include "MeshFilter.h"
#include "Interactable.h"
#include "Camera.h"
#include <glm/gtx/string_cast.hpp>
#include "Transform.h"
#include "KeyboardController.h"
#include "Terrain.h"
#include "Utils/Input.h"
#include "CameraController.h"
#include "Core/Log.h"
#include <GLFW/glfw3.h>

BumperCarController::BumperCarController(GameObject* go)
	:Component(go)
{
	rigidBody_ = go->GetComponent<RigidBody>();
	targetTransform_ = go->GetTransform();
}

void BumperCarController::Update(double dt)
{
	if (isDirty_)
	{
		rigidBody_ = gameObject_->GetComponent<RigidBody>();
		isDirty_ = false;
	}

	deltaTime_ = static_cast<float>(dt);
	if (targetTransform_->GetPosition().x > 200 || targetTransform_->GetPosition().x < -200 || targetTransform_->GetPosition().z > 200 || targetTransform_->GetPosition().z < -200)
	{
		targetTransform_->SetPosition({10, 60, 10});
		rigidBody_->SetVelocity({ 0, 0, 0 });
	}

	HandleKeyPress();
}

void BumperCarController::HandleKeyPress()
{
	if (!rigidBody_)
		return;
	
	constexpr glm::vec3 FRONT = { 0, 0, -1 }; 
	constexpr float ROTATE_SPEED = 25.f;

	static Input* kc_instance = Input::GetInstance();


	if (kc_instance->IsKeyDown('A'))
	{
		rotation_velocity += ROTATE_SPEED * deltaTime_;
	}

	if (kc_instance->IsKeyDown('D'))
	{
		rotation_velocity -= ROTATE_SPEED * deltaTime_;
	}

	rotation.y += rotation_velocity * rigidBody_->GetInverseMass(); 

	if (rotation.y > 360)
		rotation.y -= 360;

	if (rotation.y < -360)
		rotation.y += 360;

	targetTransform_->SetEulerAngles(rotation);

	rotation_velocity = 0.98f * rotation_velocity;

	const auto& front = targetTransform_->GetRotation() * FRONT * 12.f;

	if (kc_instance->IsKeyDown('W'))
	{
		rigidBody_->AddForce(front, RigidBody::FORCE_MODE::FORCE);
	}

	if (kc_instance->IsKeyDown('S'))
	{
		rigidBody_->AddForce(-front, RigidBody::FORCE_MODE::FORCE);
	}

	//CC_TRACE(glm::to_string(targetTransform_->GetPosition()));
	targetTransform_->ForceUpdate();
}

void BumperCarController::BindCamera(Camera* cam)
{
	refCamera_ = cam;
}

void BumperCarController::CameraFollowTransform(Transform* other)
{
	followingCameraTransform_ = other;
}

void BumperCarController::SetCameraPositionOffset(const glm::vec3& offset)
{
	cameraOffset_ = offset;
}
