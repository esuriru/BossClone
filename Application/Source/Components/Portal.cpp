#include "Portal.h"
#include "Ray.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "Core/Application.h"

#include "Components/SphereCollider.h"

Portal::Portal(GameObject* go)
	: Component(go)
	, col_(go->AddComponent<SphereCollider>())
{
	col_->SetRadius(3.0f);
	col_->SetOffset({ 0, -2, 0 });
}

void Portal::Update(double dt)
{
	static double timer = 0.0;
	timer += dt;

	if (timer >= glm::pi<double>())
		timer -= glm::pi<double>();

	gameObject_->GetTransform()->SetScale({ sin(timer) * 2 + 10, sin(timer) * 2 + 15, 1 });

	const auto& colliding = col_->GetActivelyColliding();
	for (auto& c : colliding)
	{
		auto sc = dynamic_cast<Ray*>(c);
		if (c->GetGameObject()->tag == "Player" && sc == nullptr)
		{
			if (sceneName == "NULL")
			{
				CC_WARN("No scene loaded up in this portal. Not going through with teleport.");
				return;
			}
			Application::GetInstance()->QueueSceneLoad(sceneName);
			return;
		}
	}
}
