#include "CoveringRock.h"
#include "RigidBody.h"
#include "Scene/GameObject.h"
#include "Collider.h"
#include "Physics/ColliderManager.h"
#include <iostream>

CoveringRock::CoveringRock(GameObject* go)
	: Component(go)
	, targetCollider_(go->GetComponent<Collider>())
	, hidingObject_(nullptr)
{
	
}

void CoveringRock::Update(double dt)
{
	if (isDirty_)
	{
		targetCollider_ = gameObject_->GetComponent<Collider>();
		isDirty_ = false;
	}

	if (!targetCollider_)
		return;

	static ColliderManager* cm = ColliderManager::GetInstance();
	const auto& colliding = cm->RetrieveCollisionData(targetCollider_);


	for (auto& c : colliding)
	{
		if (c->isTrigger)
		{
			gameObject_->SetActive(false);

			if (hidingObject_)
			{
				hidingObject_->SetActive(true);
				//_hidingObject->GetTransform()->SetParent(nullptr);
				const auto& col = hidingObject_->GetComponent<Collider>();
				if (col)
				{
					col->Initialize();
					const auto& rb =hidingObject_->GetComponent<RigidBody>();
					if (rb)
					{
						col->attachedRigidbody = rb;
						rb->Update(0);
					}
				}
			}
		}
	}
}

void CoveringRock::BindObject(std::shared_ptr<GameObject> objectToHide)
{
	hidingObject_ = objectToHide;
	// ?
	hidingObject_->SetActive(false);
	hidingObject_->GetTransform()->SetPosition(gameObject_->GetTransform()->GetPosition());
}
