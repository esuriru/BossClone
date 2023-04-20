#include "Collision.h"
#include "Core/Core.h"
#include <sstream>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "MyMath.h"
#include "Components/Collider.h"
#include "Components/RigidBody.h"
#include "Core/Log.h"

void Collision::SendCollisionEvent()
{
	ResolveCollision();
}

void Collision::ResolveCollision()
{
	//std::cout << "Collision detected.\n";
#if 1
	if (!rigidbody && !otherRigidbody) return;
	glm::vec3 relativeVelocity;
	glm::vec3 evaluatedImpulse;

	// Elasticity
	float e;
	float velocityAlongNormal;

	if (rigidbody && otherRigidbody && rigidbody->GetType() != RigidBody::RIGID_BODY_TYPE::STATIC &&otherRigidbody->GetType() != RigidBody::RIGID_BODY_TYPE::STATIC )
	{
		float combinedInverseMass = rigidbody->GetInverseMass() + otherRigidbody->GetInverseMass();
		e = Math::Min(rigidbody->GetRestitution(), otherRigidbody->GetRestitution());

		//relativeVelocity = otherRigidbody->GetVelocity()- rigidbody->GetVelocity();
		relativeVelocity = rigidbody->GetVelocity() - otherRigidbody->GetVelocity();
		velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);
		evaluatedImpulse = velocityAlongNormal * (-(1 + e)) * contact.normal;
		evaluatedImpulse *= 1 / (combinedInverseMass);


		const auto& mainPenDepth = penetrationDepth * (otherRigidbody->GetInverseMass() / combinedInverseMass);
		const auto& otherPenDepth = penetrationDepth - mainPenDepth;
		collider->SetPosition(collider->GetPosition() + contact.normal * mainPenDepth); // resolve penetration
		otherCollider->SetPosition(otherCollider->GetPosition() -contact.normal * otherPenDepth); // resolve penetration

		//CC_TRACE("Two Rigidbody Collision: Collider final position: ", glm::to_string(collider->GetPosition()));
		//CC_ASSERT(!isnan(collider->GetPosition().x), "Not a number.");
		//CC_TRACE("Two Rigidbody Collision: Other Collider final position: ", glm::to_string(otherCollider->GetPosition()));

		rigidbody->AddForce(evaluatedImpulse, RigidBody::FORCE_MODE::IMPULSE);
		otherRigidbody->AddForce(-evaluatedImpulse, RigidBody::FORCE_MODE::IMPULSE);

		
	}
	else
	{
		if (rigidbody && rigidbody->GetType() != RigidBody::RIGID_BODY_TYPE::STATIC && (!otherRigidbody || otherRigidbody->GetType() == RigidBody::RIGID_BODY_TYPE::STATIC))
		{
			e = rigidbody->GetRestitution();

			relativeVelocity = rigidbody->GetVelocity();
			velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);
			evaluatedImpulse = velocityAlongNormal * (-(1 + e)) * contact.normal;
			evaluatedImpulse *= 1 / (rigidbody->GetInverseMass());
			
			collider->SetPosition(collider->GetPosition() +(contact.normal * penetrationDepth)); // resolve penetration
			rigidbody->AddForce(evaluatedImpulse, RigidBody::FORCE_MODE::IMPULSE);

			//std::cout << penetrationDepth << "\n";
		}
		else if ((!rigidbody || rigidbody->GetType() == RigidBody::RIGID_BODY_TYPE::STATIC) && otherRigidbody->GetType() != RigidBody::RIGID_BODY_TYPE::STATIC)
		{
			contact.normal = -contact.normal;
			e = otherRigidbody->GetRestitution();

			relativeVelocity = otherRigidbody->GetVelocity();
			velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);
			evaluatedImpulse = velocityAlongNormal * (- (1 + e))* contact.normal;
			evaluatedImpulse *= 1 / (otherRigidbody->GetInverseMass());
			
			otherCollider->SetPosition(otherCollider->GetPosition() +(contact.normal * penetrationDepth)); // resolve penetration
			otherRigidbody->AddForce(evaluatedImpulse, RigidBody::FORCE_MODE::IMPULSE);
		}
	}
#endif
}

void Collision::SetColliders(Collider* main, Collider* other)
{
	collider = main;
	otherCollider = other;
	rigidbody = main->attachedRigidbody;
	otherRigidbody = other->attachedRigidbody;
}

std::string Collision::ToString() const
{
	std::stringstream ss;
	ss << "Collision normal: " << glm::to_string(contact.normal);
	ss << "Collision point: " << glm::to_string(contact.point);
	return ss.str();
}
