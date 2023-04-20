#pragma once
#include "Contact.h"
#include <string>

class Collider;
class RigidBody;

struct Collision
{
	bool handled = false;
	bool enabled = true;

	Contact contact;
	Collider* collider = nullptr;
	Collider* otherCollider = nullptr;
	RigidBody* rigidbody = nullptr;
	RigidBody* otherRigidbody = nullptr;
	//Transform* transform;
	float penetrationDepth;

	void SendCollisionEvent();
	void ResolveCollision();

	void SetColliders(Collider* main, Collider* other);

	std::string ToString() const;

	Collision() = default;

};