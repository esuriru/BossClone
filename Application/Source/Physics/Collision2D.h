#pragma once
#include "Contact.h"


class RigidBody2D;
class Collider2D;

/**
 * \brief Collision detail data
 */
struct Collision2D
{
public:
	bool handled = false;
	bool enabled = true;
	Contact contact;
	Collider2D* collider = nullptr;
	Collider2D* otherCollider = nullptr;
	RigidBody2D * rigidbody = nullptr;
	RigidBody2D* otherRigidbody = nullptr;
	//Transform* transform;
	float penetrationDepth;

	void SendCollisionEvent();
	void ResolveCollision();

	void SetColliders(Collider2D* main, Collider2D* other);

	Collision2D();

};

