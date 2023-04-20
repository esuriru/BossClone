#include "RigidBody.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "Scene/GameObject.h"
#include <exception>
#include "Physics/ColliderManager.h"
#include "Components/Collider.h"
#include "Physics/Collision.h"
#include "Core/Log.h"


constexpr float COEFFICIENT_OF_FRICTION = 0.98f;
constexpr float time_scale = 2.5f;

RigidBody::RigidBody(GameObject* go)
	: Component(go)
	,mass_(1)
	,inverseMass_(1/mass_)
	,velocity_(.0f, .0f, .0f)
	,targetTransform_(go->GetTransform())
	,targetCollider_(go->GetComponent<Collider>())
	,bodyType_(RIGID_BODY_TYPE::DYNAMIC)
	,restitution_(0.2f)
{
}

bool RigidBody::HandleCollision()
{
	for (auto& col : targetCollider_->unhandledCollisions)
	{
		if (col.second.handled)
			continue;

		col.second.SendCollisionEvent();

		velocity_ = COEFFICIENT_OF_FRICTION * velocity_;
		col.second.handled = true;
	}

	targetCollider_->unhandledCollisions.clear();
	return true;
}

void RigidBody::Update(double dt)
{
	if (isDirty_)
	{
		targetCollider_ = gameObject_->GetComponent<Collider>();
		isDirty_ = false;
	}

	//if (targetTransform_->GetPosition().y < -100)
	//{
	//	targetTransform_->Translate({ 0, 100, 0 });
	//	targetCollider_->Translate({ 0, 100, 0 });
	//}
}

void RigidBody::FixedUpdate()
{
	if (!targetCollider_)
		return;
	HandleCollision();
	targetTransform_->SetPosition(targetCollider_->GetPosition());
	targetTransform_->ForceUpdate();
}

void RigidBody::AddForce(glm::vec3 force, FORCE_MODE mode)
{
	if (bodyType_ == RIGID_BODY_TYPE::KINEMATIC) return;

	switch (mode)
	{
	case FORCE_MODE::ACCELERATION:
		velocity_ += force * deltaTime_ * time_scale;
		break;
	case FORCE_MODE::FORCE:
		velocity_ += force * deltaTime_ * time_scale * inverseMass_;
		break;
	case FORCE_MODE::IMPULSE:
		velocity_ += force * inverseMass_;
		break;
	case FORCE_MODE::VELOCITY_CHANGE:
		velocity_ += force;
		break;
	}

}

void RigidBody::UpdateGravity()
{
	deltaTime_ = static_cast<float>(FIXED_UPDATE_FRAME_TIME);
	AddForce(gravity_acceleration, FORCE_MODE::ACCELERATION);

	// Gravity
	//auto initialVelocity = _velocity;

	if (!targetCollider_)
		return;


	targetCollider_->SetPosition(targetTransform_->GetPosition() + deltaTime_ * time_scale * (velocity_));

	//std::cout << "Velocity: " << glm::to_string(_velocity) << "\n";
}

//void RigidBody::OnCollisionEnter2D(Collision2D col)
//{
//}

//std::vector<Collider2D*> RigidBody::GetActivelyColliding() const
//{
//	return _activelyCollidingWith;
//}

float RigidBody::GetRestitution() const
{
	return restitution_;
}

glm::vec3 RigidBody::GetVelocity() const
{
	return velocity_;
}

float RigidBody::GetInverseMass() const
{
	return inverseMass_;
}

float RigidBody::GetMass() const
{
	return mass_;
}

void RigidBody::SetRestitution(float r)
{
	restitution_ = r;
}

void RigidBody::SetMass(float f)
{
	mass_ = f;
	inverseMass_ = 1 / f;
}

void RigidBody::SetVelocity(const glm::vec3& rhs) 
{
	velocity_ = rhs;
}

void RigidBody::ClearCollision()
{
	targetCollider_->unhandledCollisions.clear();
}
