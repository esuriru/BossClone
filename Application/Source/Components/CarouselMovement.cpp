#include "CarouselMovement.h"
#include "Scene/GameObject.h"
#include "Transform.h"
#include "Utils/Input.h"
#include <glm/gtx/string_cast.hpp>
#include "Core/Log.h"

CarouselMovement::CarouselMovement(GameObject* go)
	: Component(go)
	, targetTransform_(go->GetTransform())
{
}

void CarouselMovement::Update(double dt)
{
	targetTransform_->Rotate(glm::angleAxis(glm::radians(13.f * static_cast<float>(dt)), glm::vec3(0, 1, 0)));
}
