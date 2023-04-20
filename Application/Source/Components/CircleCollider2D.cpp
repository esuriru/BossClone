#include "CircleCollider2D.h"
#include "BoxCollider2D.h"
#include "Scene/GameObject.h"
#include "BoundingCircle.h"

CircleCollider2D::CircleCollider2D(GameObject* go)
    : Collider2D(go),
    radius_(go->GetTransform()->GetScale().x)
{
	position_ = targetTransform_->GetPosition();
    boundingVolume = new BoundingCircle(this, position_, radius_);
}

float CircleCollider2D::GetRadius(void) const
{
    return radius_;
}

bool CircleCollider2D::IsColliding(const Collider2D* c) const
{
    //return c->IsColliding(this);
	return c->boundingVolume->IsIntersecting(this->boundingVolume);
}

void CircleCollider2D::Rotate(float degrees)
{

}


void CircleCollider2D::Update(double dt)
{
	if (isDirty_)
	{
		RetrieveRigidBody();
		isDirty_ = false;
	}

	if (!attachedRigidbody)
	{
		SetPosition(targetTransform_->GetPosition());
	}
	//boundingVolume->centre = _position + _offset;
}
