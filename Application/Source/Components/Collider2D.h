#pragma once
#include "Component.h"
#include <glm/gtc/quaternion.hpp>
#include <unordered_map>
#include <deque>
#include <glm/glm.hpp>
#include "Transform.h"
#include "Physics/Collision2D.h"

class BoundingVolume2D;
class ColliderManager;
class BoxCollider2D;
class CircleCollider2D;
class RigidBody2D;

class Collider2D : public Component
{
public:
	Collider2D(GameObject* go);
	virtual ~Collider2D();

	virtual void Update(double dt) = 0;
	virtual bool IsColliding(const Collider2D* c) const;
	glm::vec2 GetPosition() const;
	glm::vec2 GetOffset() const;

	void Translate(const glm::vec2& trans);
	virtual void SetPosition(const glm::vec2& pos);
	void SetOffset(const glm::vec2& offset);
	virtual void Rotate(float clockwiseRotationDegrees);
	
	virtual void Rotate(const glm::quat& rot);

	BoundingVolume2D* boundingVolume;

	RigidBody2D* attachedRigidbody;

	std::unordered_map<Collider2D*, Collision2D> unhandledCollisions;

	bool isTrigger;
protected:
	Transform* targetTransform_;


	glm::vec2 offset_;
	glm::vec2 position_;

	friend ColliderManager;

	void RetrieveRigidBody();
};

