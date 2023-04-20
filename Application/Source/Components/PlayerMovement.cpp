#include "PlayerMovement.h"
#include "Core/Application.h"
#include "Scene/GameObject.h"
#include "RigidBody.h"

//constexpr float reciprocal = 1 / 40.f;

PlayerMovement::PlayerMovement(GameObject* go)
	: Component(go)
	, targetBody_(go->GetComponent<RigidBody>())
{

}

void PlayerMovement::Update(double dt)
{
	if (isDirty_)
	{
		targetBody_ = gameObject_->GetComponent<RigidBody>();
		isDirty_ = false;
	}
	deltaTime_ = static_cast<float>(dt);
	HandleKeyPress();
	
}

void PlayerMovement::HandleKeyPress()
{
	// This "input" should be a Input static class instead
	// Is key pressed means IsKeyDown
	//if (Application::IsKeyPressed('A'))
	//{
	//	static float rotation = 0;
	//	rotation += 0.01f;
	//	_gameObject->GetTransform()->SetRotation({ 0, rotation, rotation });
	//}

	if (!targetBody_)
		return;
	
	if (Application::IsKeyPressed('W'))
	{
		targetBody_->AddForce({ 1.f, 0, 0 });
	}

	if (Application::IsKeyPressed('S'))
	{
		targetBody_->AddForce({ -1.f, 0, 0 });
	}

	if (Application::IsKeyPressed('A'))
	{
		targetBody_->AddForce({ 0.00f, 0.00f, 1.f });
	}

	if (Application::IsKeyPressed('D'))
	{
		targetBody_->AddForce({ 0.00f, 0.0f, -1.f });
	}

}
