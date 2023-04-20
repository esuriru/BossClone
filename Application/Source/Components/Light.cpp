#include "Light.h"

#include "Scene/GameObject.h"

Light::Light(GameObject* go)
	: Component(go)
	, data(new LightData())
{
	data->type = LightType::Directional;
	data->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	data->power = 1.0f;
}

Light::~Light()
{
	if (data)
		delete data;
	data = nullptr;
}

void Light::Update(double dt)
{
	data->centre = glm::vec4(gameObject_->GetTransform()->GetWorldPosition(), 1.0f);
}

