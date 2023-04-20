#pragma once
#include "Component.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include "Physics/Collision.h"


class BoundingVolume;
class RigidBody;



class Collider : public Component
{
public:
	Collider(GameObject* go);
	virtual ~Collider();

	virtual void Update(double dt) = 0;
	virtual bool IsColliding(const Collider* c) const;


	void Translate(const glm::vec3& trans);
	void SetPosition(const glm::vec3& pos);
	void SetOffset(const glm::vec3& offset);
	glm::vec3 GetPosition() const;
	glm::vec3 GetOffset() const;

	virtual void Initialize();

	std::vector<Collider*> GetActivelyColliding() const;

	BoundingVolume* boundingVolume;
	RigidBody* attachedRigidbody;

	//Collision unhandledCollision;
	std::unordered_map<Collider*, Collision> unhandledCollisions;

	bool isTrigger = false;
protected:
	glm::vec3 position_;
	glm::vec3 offset_;

};

