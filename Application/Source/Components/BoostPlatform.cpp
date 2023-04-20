#include "BoostPlatform.h"

#include "Physics/ColliderManager.h"
#include "Scene/GameObject.h"
#include "RigidBody2D.h"

BoostPlatform::BoostPlatform(GameObject* go)
	: Component(go)
	, forceToAddOn_({0, 1, 0})
	, col_(go->GetComponent<BoxCollider2D>())
	, platformBase_(nullptr)
{
}

void BoostPlatform::Update(double dt)
{
	if (isDirty_)
	{
		col_ = gameObject_->GetComponent<BoxCollider2D>();
		isDirty_ = false;
	}
	// Ignore the first one
	if (!platformBase_) return;
	const auto& colliders = ColliderManager::GetInstance()->RetrieveCollisionDataIgnoringSingular(col_, platformBase_);
	for (unsigned int i = 0; i <  colliders.size(); ++i)
	{
		if (colliders[i]->attachedRigidbody)
		{
			colliders[i]->attachedRigidbody->AddForce(forceToAddOn_, RigidBody2D::FORCE_MODE::IMPULSE);
		}
	}
}

void BoostPlatform::SetForce(const glm::vec3& force)
{
	forceToAddOn_ = force;
}

void BoostPlatform::SetPlatformBase(Collider2D* platformBase)
{
	platformBase_ = platformBase;
}
