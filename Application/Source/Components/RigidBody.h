#pragma once
#include "Component.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>
#include "Transform.h"
#include "Physics/Collision.h"

class Collider;

constexpr glm::vec3 gravity_acceleration{0, -9.8f, 0};

class RigidBody : public Component
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
	RigidBody(GameObject* go);

	virtual void Update(double dt) override;
	void FixedUpdate() override;
	void AddForce(glm::vec3 force, FORCE_MODE mode = FORCE_MODE::FORCE);
	void UpdateGravity() override;
	//std::vector<Collider2D*> GetActivelyColliding(void) const;
	
	float GetRestitution(void) const;
	glm::vec3 GetVelocity(void) const;
	float GetInverseMass(void) const;
	float GetMass(void) const;
	inline RigidBody::RIGID_BODY_TYPE GetType() const
	{
		return bodyType_;
	}

	void SetRestitution(float r);
	void SetMass(float f);
	void SetVelocity(const glm::vec3& rhs); // should never use this until we have to.

	void ClearCollision();

protected:
	bool HandleCollision();

	std::vector<Collider*> activelyCollidingWith_;
	float deltaTime_;

	float mass_;
	float inverseMass_;
	
	glm::vec3 velocity_;
	Transform* targetTransform_;
	Collider* targetCollider_;
	RIGID_BODY_TYPE bodyType_;
	float restitution_;
	
};

