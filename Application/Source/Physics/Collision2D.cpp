#include "Collision2D.h"
#include "Utils/Util.h"
#include "Components/Collider2D.h"

#include "Components/BoundingVolume2D.h"
#include "Components/RigidBody2D.h"
#include <glm/glm.hpp>
#include <glm/common.hpp>

#define DEBUG_LOG(object) if (_DEBUG) std::cout << (object) << "\n";

void Collision2D::SendCollisionEvent()
{
	ResolveCollision();
	if (rigidbody)
		rigidbody->OnCollisionEnter2D(*this);
	if (otherRigidbody)
		otherRigidbody->OnCollisionEnter2D(*this);
	//DEBUG_LOG("Collision resolved")
}

void Collision2D::ResolveCollision()
{

	if (!rigidbody && !otherRigidbody) return;
	glm::vec3 relativeVelocity;
	glm::vec3 evaluatedImpulse;

	// Elasticity
	float e;
	float velocityAlongNormal;
	if (rigidbody && otherRigidbody)
	{
		float combinedMass = rigidbody->GetMass() + otherRigidbody->GetMass();
		e = glm::min(rigidbody->GetRestitution(), otherRigidbody->GetRestitution());

#if 1
		glm::vec2 rA = contact.point - glm::vec3(collider->GetPosition(), 0);
		glm::vec2 rB = contact.point - glm::vec3(otherCollider->GetPosition(), 0);

		glm::vec2 rpA = Util::PerpendicularClockwise(rA);
		glm::vec2 rpB = Util::PerpendicularClockwise(rB);
		

		glm::vec2 angularLinearVelA =  rpA * rigidbody->GetAngularVelocity().z;
		glm::vec2 angularLinearVelB =  rpB * rigidbody->GetAngularVelocity().z;


		relativeVelocity = (rigidbody->GetVelocity() + glm::vec3(angularLinearVelA, 0))
			- (glm::vec3(angularLinearVelB, 0) + otherRigidbody->GetVelocity());
		velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);
		float angularVelA = glm::dot(rpA, glm::vec2(contact.normal));
		float angularVelB = glm::dot(rpB, glm::vec2(contact.normal));
		evaluatedImpulse = velocityAlongNormal * (-(1 + e)) * contact.normal;
		float denominator = rigidbody->GetInverseMass() + otherRigidbody->GetInverseMass() +
			(angularVelA * angularVelA) * rigidbody->GetInverseInertia() +
			(angularVelB * angularVelB) * otherRigidbody->GetInverseInertia();
		evaluatedImpulse /= denominator;
#else
		relativeVelocity = rigidbody->GetVelocity() - otherRigidbody->GetVelocity();
		velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);
		evaluatedImpulse = velocityAlongNormal * (-(1 + e)) * contact.normal;
		float denominator = rigidbody->GetInverseMass() + otherRigidbody->GetInverseMass();
		evaluatedImpulse /= denominator;
#endif

		const auto& mainPenDepth = penetrationDepth * (otherRigidbody->GetMass() / combinedMass);
		const auto& otherPenDepth = penetrationDepth - mainPenDepth;

		collider->Translate(contact.normal * mainPenDepth); // resolve penetration
		otherCollider->Translate(-contact.normal * otherPenDepth); // resolve penetration
		rigidbody->AddForce(evaluatedImpulse, RigidBody2D::FORCE_MODE::IMPULSE);
		otherRigidbody->AddForce(-evaluatedImpulse, RigidBody2D::FORCE_MODE::IMPULSE);
#if 1
		rigidbody->SetAngularVelocity(rigidbody->GetAngularVelocity()
			+ glm::vec3(0, 0, Util::Cross(rA, glm::vec2(-evaluatedImpulse)) * rigidbody->GetInverseInertia()));
		otherRigidbody->SetAngularVelocity(otherRigidbody->GetAngularVelocity()
			+ glm::vec3(0, 0, Util::Cross(rB, glm::vec2(evaluatedImpulse)) * otherRigidbody->GetInverseInertia()));
#endif

	}
	else
	{
		if (rigidbody)
		{
			e = rigidbody->GetRestitution();

			glm::vec2 rA = contact.point - glm::vec3(collider->GetPosition(), 0);
			glm::vec2 rpA = Util::PerpendicularClockwise(rA);
			float angularVelA = glm::dot(rpA, glm::vec2(contact.normal));
#if 1
			relativeVelocity = rigidbody->GetVelocity() + glm::vec3(rpA, 0);
			velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);

			//if (velocityAlongNormal < 0)
			//	return;
			evaluatedImpulse = velocityAlongNormal * (-(1 + e)) * contact.normal;
			evaluatedImpulse *= 1 / (rigidbody->GetInverseMass() + (angularVelA * angularVelA) * rigidbody->GetInverseInertia());

#else
			relativeVelocity = rigidbody->GetVelocity();
			velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);
			evaluatedImpulse = velocityAlongNormal * (-(1 + e)) * contact.normal;
			evaluatedImpulse *= 1 / (rigidbody->GetInverseMass());
#endif
			


			collider->Translate(contact.normal * penetrationDepth); // resolve penetration
			rigidbody->AddForce(evaluatedImpulse, RigidBody2D::FORCE_MODE::IMPULSE);
#if 1
			rigidbody->SetAngularVelocity(rigidbody->GetAngularVelocity()
				+ glm::vec3(0, 0, -Util::Cross(rA, glm::vec2(evaluatedImpulse)) * rigidbody->GetInverseInertia()));
#endif
		}
		else
		{
			e = otherRigidbody->GetRestitution();
			contact.normal = -contact.normal;

			glm::vec2 rA = contact.point - glm::vec3(otherCollider->GetPosition(), 0);
			glm::vec2 rpA = Util::PerpendicularClockwise(rA);
			float angularVelA = glm::dot(rpA, glm::vec2(contact.normal));
#if 1
			relativeVelocity = otherRigidbody->GetVelocity() + glm::vec3(rpA, 0);
			velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);
			//if (velocityAlongNormal < 0)
			//	return;
			evaluatedImpulse = velocityAlongNormal * (-(1 + e)) * contact.normal;
			evaluatedImpulse *= 1 / (otherRigidbody->GetInverseMass() + (angularVelA * angularVelA) * otherRigidbody->GetInverseInertia());
#else
			relativeVelocity = otherRigidbody->GetVelocity();
			velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);
			evaluatedImpulse = velocityAlongNormal * (-(1 + e)) * contact.normal;
			evaluatedImpulse *= 1 / (otherRigidbody->GetInverseMass());
#endif
			
			otherCollider->Translate(contact.normal * penetrationDepth); // resolve penetration
			otherRigidbody->AddForce(evaluatedImpulse, RigidBody2D::FORCE_MODE::IMPULSE);
#if 1
			otherRigidbody->SetAngularVelocity(otherRigidbody->GetAngularVelocity()
				+ glm::vec3(0, 0, Util::Cross(rA, glm::vec2(evaluatedImpulse)) * otherRigidbody->GetInverseInertia()));
#endif
		}
	}
}

void Collision2D::SetColliders(Collider2D* main, Collider2D* other)
{
	collider = main;
	otherCollider = other;
	rigidbody = main->attachedRigidbody;
	otherRigidbody = other->attachedRigidbody;
}

Collision2D::Collision2D()
	: enabled(true)
{
}
