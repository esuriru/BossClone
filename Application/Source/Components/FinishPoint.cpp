#include "FinishPoint.h"
#include "Scene/MainScene.h"
#include "Core/Application.h"
#include "RigidBody.h"

#include "Components/Collider.h"
#include "Collider2D.h"
#include "Physics/ColliderManager.h"
#include "Scene/GameObject.h"
#include "FallingObject.h"
#include "Renderer/SceneRenderer.h"

FinishPoint::FinishPoint(GameObject* go)
	: Component(go)
	, target_(go->GetComponent<Collider>())
	, redIn_(false)
	, greenIn_(false)
	, blueIn_(false)
	, hasWon_(false)
{

}

void FinishPoint::Update(double dt)
{
	if (isDirty_)
	{
		target_ = gameObject_->GetComponent<Collider>();
		isDirty_ = false;
	}
	if (!target_)
		return;
	bool win = redIn_ && greenIn_ && blueIn_;
	const auto& colliding = target_->GetActivelyColliding();

	for (const auto& c : colliding)
	{
		if (c->GetGameObject()->tag == "Red" && !redIn_)
		{
			redIn_ = true;
			c->attachedRigidbody->SetActive(false);
			c->SetActive(false);
		}
		if (c->GetGameObject()->tag == "Green" && !greenIn_)
		{
			greenIn_ = true;
			c->attachedRigidbody->SetActive(false);
			c->SetActive(false);
		}
		if (c->GetGameObject()->tag == "Blue" && !blueIn_)
		{
			blueIn_ = true;
			c->attachedRigidbody->SetActive(false);
			c->SetActive(false);
		}
	}

	if (win && !hasWon_)
	{
		hasWon_ = true;
		if (confetti_)
			confetti_->SetActive(true);
		CC_INFO("Scene unlocked!");
		//Application::GetInstance()->QueueSceneLoad("JungleScene");
		dynamic_cast<MainScene&>(Application::GetInstance()->GetActiveScene()).bumperCarSceneEnabled = true;

		if (bindedObject_)
		{
			bindedObject_->SetActive(true);
			bindedObject_->Dirty();
			// the portal
			auto lightComp = bindedObject_->GetComponent<Light>();
			if (lightComp)
			{
				lightComp->data->power = 1.0f;
			}
		}
    }
}

void FinishPoint::BindObject(GameObject* obj)
{
	bindedObject_ = obj;
	obj->SetActive(false);
}

void FinishPoint::BindConfetti(GameObject* con)
{
	confetti_ = con;
	con->SetActive(false);
}
