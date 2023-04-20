#include "ForceDebug.h"
#include "Utils/Input.h"
#include "RigidBody.h"
#include "Scene/GameObject.h"

ForceDebug::ForceDebug(GameObject* go)
	: Component(go)
{
	rigidbody_ = go->GetComponent<RigidBody>();
}

void ForceDebug::UpdateGravity()
{
	if (isDirty_)
	{
		rigidbody_ = gameObject_->GetComponent<RigidBody>();
		isDirty_ = false;
	}

}

void ForceDebug::FixedUpdate()
{
	static Input* input = Input::GetInstance();
	if (!rigidbody_)
		return;
	if (input->IsKeyDown('W'))
	{
		rigidbody_->AddForce(glm::vec3(10.0f, 0.0f, 0), RigidBody::FORCE_MODE::FORCE);
	}
	if (input->IsKeyDown('S'))
	{
		rigidbody_->AddForce(glm::vec3(-10.0f, 0.0f, 0), RigidBody::FORCE_MODE::FORCE);
	}
	if (input->IsKeyDown('D'))
	{
		rigidbody_->AddForce(glm::vec3(0.0f, 0.0f, 10.0f), RigidBody::FORCE_MODE::FORCE);
	}
	if (input->IsKeyDown('A'))
	{
		rigidbody_->AddForce(glm::vec3(0.0f, 0.0f, -10.0f), RigidBody::FORCE_MODE::FORCE);
	}
}