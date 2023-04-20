#include "DebugMovement.h"
#include "Scene/GameObject.h"
#include "Transform.h"
#include "Utils/Input.h"
#include <glm/gtx/string_cast.hpp>
#include "Core/Log.h"

DebugMovement::DebugMovement(GameObject* go)
	: Component(go)
	, targetTransform_(go->GetTransform())
{
}

void DebugMovement::Update(double dt)
{
	static Input* input = Input::GetInstance();
	if (input->GetInstance()->IsKeyDown('R'))
		targetTransform_->Translate(glm::vec3(0, 5.5f * dt, 0));
	if (input->GetInstance()->IsKeyDown('Y'))
		targetTransform_->Translate(glm::vec3(0, -5.5f * dt, 0));
	if (input->GetInstance()->IsKeyDown('T'))
		targetTransform_->Translate(glm::vec3(5.5f * dt, 0, 0));
	if (input->GetInstance()->IsKeyDown('G'))
		targetTransform_->Translate(glm::vec3(-5.5 * dt, 0, 0));
	if (input->GetInstance()->IsKeyDown('F'))
		targetTransform_->Translate(glm::vec3(0, 0, 5.5 * dt));
	if (input->GetInstance()->IsKeyDown('H'))
		targetTransform_->Translate(glm::vec3(0, 0, -5.5f * dt));

	static bool displayPosition = false;
	if (input->GetInstance()->IsKeyPressed('M'))
		displayPosition = !displayPosition;
	if (displayPosition)
	{
		CC_TRACE(glm::to_string(targetTransform_->GetWorldPosition()));
	}
}
