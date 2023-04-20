#include "SpotlightController.h"
#include "Scene/GameObject.h"
#include "Ray.h"

SpotlightController::SpotlightController(GameObject* go)
	: Component(go)
	, attachedLight_(go->GetComponent<Light>())
	, ray_(nullptr)
{
}

void SpotlightController::Update(double dt)
{
	if (isDirty_)
	{
		attachedLight_ = gameObject_->GetComponent<Light>();
		isDirty_ = false;
	}

	if (!attachedLight_)
		return;

	if (ray_)
		attachedLight_->data->direction = { ray_->GetDirection(), 0.f};
}


