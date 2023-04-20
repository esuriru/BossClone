#pragma once
#include <glm/glm.hpp>
#include "Component.h"
#include <vector>

constexpr glm::vec3 gravity_acceleration{0, -9.8f, 0};

class Transform;
class Collider2D;
struct Collision2D;

class RigidBody2D : public Component
{
public:
	enum class RIGID_BODY_TYPE
	{
		STATIC = 0,
		DYNAMIC,
		KINEMATIC,
		NUM_RIGID_BODY_TYPE
	};

	enum class FORCE_MODE
	{
		FORCE = 0,
		ACCELERATION,
		IMPULSE,
		VELOCITY_CHANGE
	};

	RigidBody2D(GameObject* go);
	virtual ~RigidBody2D() = default;

	void Update(double dt);
	void FixedUpdate() override;
	void AddForce(glm::vec3 force, FORCE_MODE mode = FORCE_MODE::FORCE);
	void OnCollisionEnter2D(Collision2D col);
	void UpdateGravity() override;

	std::vector<Collider2D*> GetActivelyColliding(void) const;
	
	void SetOrientation(float o);
	float GetRestitution(void) const;
	glm::vec3 GetVelocity(void) const;
	float GetInverseMass(void) const;
	float GetMass(void) const;
	float GetInverseInertia() const;

	glm::vec3 GetAngularVelocity() const;
	void SetAngularVelocity(const glm::vec3& av);

	void Reset();

	void SetRestitution(float r);
	void SetMass(float f);
	void SetVelocity(const glm::vec3& rhs); // should never use this until we have to.
	void SetInertia(float i);

	Collider2D* GetAttachedCollider() const;
	
protected:
	bool HandleCollision();

	std::vector<Collider2D*> activelyCollidingWith_;
	float deltaTime_;

	float mass_;
	float inverseMass_;
	
	glm::vec3 velocity_;
	glm::vec3 angularVelocity_;
	Transform* targetTransform_;
	Collider2D* targetCollider_;
	RIGID_BODY_TYPE bodyType_;

	float orientation_;

	float restitution_;

	float inertia_;
	float inverseInertia_;
};