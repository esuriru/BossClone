#include "ThrowableRock.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "Ray.h"
#include "Scene/GameObject.h"
#include "GLFW/glfw3.h"
#include "Collider.h"
#include "Utils/Input.h"
#include "RigidBody.h"
#include "Core/Log.h"

ThrowableRock::ThrowableRock(GameObject* go)
	: Interactable(go)
	, targetTransform_(go->GetTransform())
	, collider_(go->GetComponent<Collider>())
{
	timesUsable = 1;
	force_scalar = 20;
}

void ThrowableRock::Update(double dt)
{
	if (isDirty_)
	{
		collider_ = gameObject_->GetComponent<Collider>();
	}
	HandleKeyPress();
}

void ThrowableRock::Use(UseData& data)
{
	// TODO - Might need to fix position and other stuff.
	targetTransform_->SetParent(nullptr);
	collider_->SetPosition(targetTransform_->GetPosition());

	//// There must be a collider. Might make this fatal.
	RigidBody* rb = nullptr;
	if (collider_)
	{
		collider_->SetActive(true);
		if (collider_->attachedRigidbody)
		{
			rb = collider_->attachedRigidbody;
			rb->SetActive(true);
			//rb->SetVelocity({0, 0, 0});
		}
	}

	data.playerItemHolding = nullptr;
	if (!rb)
		return;


	// TODO - Might make this change.
	//std::cout << "Throwing direction: " << glm::to_string(data.useRay->GetDirection()) << "\n";
	rb->ClearCollision();
	rb->SetVelocity(data.useRay->GetDirection() * force_scalar);
}

void ThrowableRock::HandleKeyPress()
{
	static Input* kc_instance = Input::GetInstance();
	if (kc_instance->IsKeyDown(GLFW_KEY_X))
	{
		if (force_scalar < 50)
		force_scalar += 1;
		CC_INFO(std::to_string(force_scalar));
	}
	if (kc_instance->IsKeyDown(GLFW_KEY_Z))
	{
		if (force_scalar > 10)
			force_scalar -= 1;
	}
}
