#include "Collider.h"
#include "BoundingVolume.h"
#include "Physics/ColliderManager.h"
#include "RigidBody.h"
#include "Scene/GameObject.h"

static ColliderManager* collider_manager = ColliderManager::GetInstance();

Collider::Collider(GameObject* go)
	: Component(go)
	, attachedRigidbody(go->GetComponent<RigidBody>())
{
	collider_manager->AddCollider(this);
}

Collider::~Collider()
{
	collider_manager->RemoveCollider(this);
	delete boundingVolume;
	boundingVolume = nullptr;
}
//
void Collider::Update(double dt)
{
	if (isDirty_)
	{
		position_ = glm::vec3(boundingVolume->GetPosition());
		isDirty_ = false;
	}
}

bool Collider::IsColliding(const Collider* c) const
{
	return c->boundingVolume->IsIntersecting(this->boundingVolume);
}

glm::vec3 Collider::GetOffset() const
{
	return offset_;
}

glm::vec3 Collider::GetPosition() const
{
	return position_;
}

void Collider::SetPosition(const glm::vec3& pos)
{
	position_ = pos;
	boundingVolume->SetPosition(position_ + offset_);
}

void Collider::SetOffset(const glm::vec3& offset)
{
	offset_ = offset;
	boundingVolume->SetPosition(position_ + offset_);
}


void Collider::Initialize()
{
	SetPosition(gameObject_->GetTransform()->GetWorldPosition() + offset_);
}

std::vector<Collider*> Collider::GetActivelyColliding() const
{
	static ColliderManager* cm = ColliderManager::GetInstance();
	return cm->RetrieveCollisionData(this);
}

void Collider::Translate(const glm::vec3& trans)
{
	position_ += trans;
	boundingVolume->SetPosition(position_ + offset_);
}
