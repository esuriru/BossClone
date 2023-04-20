#include "Collider2D.h"
#include "Physics/ColliderManager.h"
#include "Scene/GameObject.h"
#include "RigidBody2D.h"

Collider2D::Collider2D(GameObject* go)
	: Component(go),
	position_(0, 0),
	offset_(0, 0),
	targetTransform_(go->GetTransform()),
	boundingVolume(nullptr),
	attachedRigidbody(go->GetComponent<RigidBody2D>()),
	isTrigger(false)
{
	ColliderManager::GetInstance()->AddCollider(this);
}

Collider2D::~Collider2D()
{
	if (boundingVolume)
		delete boundingVolume;
}

void Collider2D::Update(double dt)
{
	if (isDirty_)
	{
		position_ = glm::vec2(targetTransform_->GetPosition());
		isDirty_ = false;
	}
}

bool Collider2D::IsColliding(const Collider2D* c) const
{
	return c->boundingVolume->IsIntersecting(this->boundingVolume);
}

glm::vec2 Collider2D::GetPosition() const
{
	return position_;
}

glm::vec2 Collider2D::GetOffset() const
{
	return offset_;
}

void Collider2D::Translate(const glm::vec2& trans)
{
	position_ += trans;
}

void Collider2D::SetPosition(const glm::vec2& pos)
{
	position_ = pos;
	boundingVolume->SetPosition(position_ + offset_);
}

void Collider2D::SetOffset(const glm::vec2& offset)
{
	offset_ = offset;
	boundingVolume->SetPosition(position_ + offset_);
}

void Collider2D::Rotate(float clockwiseRotationDegrees)
{
	// Do nothing, sphere will probably redirect here.
}

void Collider2D::Rotate(const glm::quat& rot)
{
	boundingVolume->orientation *= rot;
	//boundingVolume->isDirty = true;
	boundingVolume->Rotate(rot);
}

void Collider2D::RetrieveRigidBody()
{
	attachedRigidbody = gameObject_->GetComponent<RigidBody2D>();
} 

