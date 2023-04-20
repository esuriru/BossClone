#include "StallFloor.h"
#include "Physics/ColliderManager.h"
#include "Scene/GameObject.h"
#include "Core/Log.h"
StallFloor::StallFloor(GameObject* go)
	: Component(go)
	, collider_(go->GetComponent<Collider>())
{
	ResetBottlePosition();
	bottledown1 = false;
	bottledown2 = false;
	bottledown3 = false;
	bottle1 = nullptr;
	bottle2 = nullptr;
	bottle3 = nullptr;
}

void StallFloor::Update(double dt)
{

	const auto& colliding = collider_->GetActivelyColliding();

	for (const auto& c : colliding)
	{
		if (c->GetGameObject()->tag == "bottle1")
		{
			bottle1 = c;
			c->attachedRigidbody->SetActive(false);
			c->SetActive(false);
			bottleTransform1_ = c->GetGameObject()->GetTransform();
			bottledown1 = true;
			CC_INFO("b1");
		}
		if (c->GetGameObject()->tag == "bottle2")
		{
			bottle2 = c;
			c->attachedRigidbody->SetActive(false);
			c->SetActive(false);
			bottleTransform2_ = c->GetGameObject()->GetTransform();
			bottledown2 = true;
			CC_INFO("b2");
		}
		if (c->GetGameObject()->tag == "bottle3")
		{
			bottle3 = c;
			c->attachedRigidbody->SetActive(false);
			c->SetActive(false);
			bottleTransform3_ = c->GetGameObject()->GetTransform();
			bottledown3 = true;
			CC_INFO("b3");
		}
	}

	ResetBottlePosition();

}

void StallFloor::ResetBottlePosition()
{
	ColliderManager::GetInstance()->Update(0.0);
	if (bottledown1 == true && bottledown2 == true && bottledown3 == true)
	{
		CC_INFO("BEN");
		bottleTransform1_->SetPosition(glm::vec3(17, 5.f, 106));
		bottleTransform2_->SetPosition(glm::vec3(18, -3.f, 106));
		bottleTransform3_->SetPosition(glm::vec3(16, -3.f, 106));
		CC_INFO("1");
		bottle1->attachedRigidbody->SetVelocity(glm::vec3(0, 0, 0));
		bottle2->attachedRigidbody->SetVelocity(glm::vec3(0, 0, 0));
		bottle3->attachedRigidbody->SetVelocity(glm::vec3(0, 0, 0));
		CC_INFO("2");
		bottle1->attachedRigidbody->SetActive(true);
		bottle2->attachedRigidbody->SetActive(true);
		bottle3->attachedRigidbody->SetActive(true);
		CC_INFO("3");
		bottle1->SetActive(true);
		bottle2->SetActive(true);
		bottle3->SetActive(true);
		CC_INFO("4");
		bottle1->Initialize();
		bottle2->Initialize();
		bottle3->Initialize();
		CC_INFO("5");
		bottledown1 = false;
		bottledown2 = false;
		bottledown3 = false;
		CC_INFO("6");
	}
}