#include "RigidBody2D.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "Physics/ColliderManager.h"
#include "Scene/GameObject.h"
#include "Transform.h"
#include <exception>

#include "BoundingSphere.h"
#include "Physics/Collision2D.h"
#include "Collider2D.h"

#ifdef _DEBUG
#	define DEBUG_LOG(object) std::cout << (object) << "\n";
#else
#	define DEBUG_LOG(object)
#endif

constexpr float time_scale = 5.0f;

constexpr float COEFFICIENT_OF_FRICTION = 0.98f;

RigidBody2D::RigidBody2D(GameObject* go)
	: Component(go)
	, mass_(1)
	, inverseMass_(1/mass_)
	, velocity_(.0f, .0f, .0f)
	, targetTransform_(go->GetTransform())
	, targetCollider_(go->GetComponent<Collider2D>())
	, bodyType_(RIGID_BODY_TYPE::DYNAMIC)
	, restitution_(0.4f)
	, inertia_(100.f)
	, inverseInertia_(1.f / inertia_)
	, orientation_((targetTransform_->GetEulerAngles().z))
	, angularVelocity_({0.f})
{
}

bool RigidBody2D::HandleCollision()
{
	activelyCollidingWith_ = ColliderManager::GetInstance()->RetrieveCollisionData2D(targetCollider_);
		//std::cout << _targetCollider->unhandledCollisions.size() << "\n";
	for (auto& col : targetCollider_->unhandledCollisions)
	{
		col.second.SendCollisionEvent();

		velocity_ = COEFFICIENT_OF_FRICTION * velocity_;
		col.second.handled = true;
	}

	targetCollider_->unhandledCollisions.clear();
	return true;
}

void RigidBody2D::Update(double dt)
{
	if (isDirty_)
	{
		targetCollider_ = gameObject_->GetComponent<Collider2D>();
		targetCollider_->unhandledCollisions.clear();
		targetCollider_->Rotate((orientation_));
		isDirty_ = false;
	}
	
}

void RigidBody2D::UpdateGravity()
{
	// Gravity
	deltaTime_ = static_cast<float>(FIXED_UPDATE_FRAME_TIME);

	if (!targetCollider_)
		return;
	AddForce(gravity_acceleration, FORCE_MODE::ACCELERATION);
	angularVelocity_ = COEFFICIENT_OF_FRICTION * angularVelocity_;

	//std::cout << "Velocity before collision handling. " << glm::to_string(_velocity) << "\n";
	const auto& a = (angularVelocity_.z * deltaTime_ * time_scale);
	orientation_ += a;

	orientation_ = fmod(orientation_, 360);
	if (orientation_ < 0)
		orientation_ += 360;

	targetCollider_->Rotate(orientation_);
	targetCollider_->SetPosition(targetTransform_->GetPosition() + velocity_ * deltaTime_ * time_scale);
	//std::cout << "Position before collision handling. " << glm::to_string(_targetCollider->GetPosition()) << "\n";
}

void RigidBody2D::FixedUpdate()
{
	HandleCollision();
	std::cout << "Velocity after collision handling. " << glm::to_string(angularVelocity_) << "\n";
	targetTransform_->SetPosition(glm::vec3(targetCollider_->GetPosition(), 0));
	targetTransform_->SetEulerAngles({0, 0, (orientation_)});
	//std::cout << _orientation << "\n";

	//std::cout << "Velocity after collision handling. " << glm::to_string(_velocity) << "\n";
	//std::cout << "Position after collision handling. " << glm::to_string(_targetCollider->GetPosition()) << "\n";
}

void RigidBody2D::AddForce(glm::vec3 force, FORCE_MODE mode)
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

void RigidBody2D::OnCollisionEnter2D(Collision2D col)
{
	
}


std::vector<Collider2D*> RigidBody2D::GetActivelyColliding() const
{
	return activelyCollidingWith_;
}

void RigidBody2D::SetOrientation(float o)
{
	orientation_ = o;
}

float RigidBody2D::GetRestitution() const
{
	return restitution_;
}

glm::vec3 RigidBody2D::GetVelocity() const
{
	return velocity_;
}

float RigidBody2D::GetInverseMass() const
{
	return inverseMass_;
}

float RigidBody2D::GetMass() const
{
	return mass_;
}

float RigidBody2D::GetInverseInertia() const
{
	return inverseInertia_;
}

glm::vec3 RigidBody2D::GetAngularVelocity() const
{
	return angularVelocity_;
}

void RigidBody2D::SetAngularVelocity(const glm::vec3& av)
{
	angularVelocity_ = av;
}

void RigidBody2D::Reset()
{
	velocity_ = { 0, 0, 0 };
	angularVelocity_ = { 0, 0, 0 };
	orientation_ = 0;
	targetCollider_->boundingVolume->Reset();
	targetCollider_->boundingVolume->centre = {0, 0};
	targetCollider_->unhandledCollisions.clear();
	targetCollider_->SetPosition(glm::vec2(targetTransform_->GetPosition()));
}

void RigidBody2D::SetRestitution(float r)
{
	restitution_ = r;
}

void RigidBody2D::SetMass(float f)
{
	mass_ = f;
	inverseMass_ = 1 / f;
}

void RigidBody2D::SetVelocity(const glm::vec3& rhs) 
{
	velocity_ = rhs;
}

void RigidBody2D::SetInertia(float i)
{
	inertia_ = i;
	inverseInertia_ = 1.f / i;
}

Collider2D* RigidBody2D::GetAttachedCollider() const
{
	return targetCollider_;
}
