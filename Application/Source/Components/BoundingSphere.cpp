#include "BoundingSphere.h"
#include "Core/Log.h"
#include "RigidBody.h"
#include "RayVolume.h"
#include <glm/gtx/norm.hpp>
#include "TerrainVolume.h"
#include "Physics/Collision.h"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/constants.hpp>
#include "AABB.h"

BoundingSphere::BoundingSphere(Collider* parent, float radius)
	: BoundingVolume(parent)
	, radius(radius)
{
}

bool BoundingSphere::IsIntersecting(const TerrainVolume* other) const
{
	const auto terrain_height_normal = other->GetHeightAndNormal(centre_.x, centre_.z);
	auto penetrationDepth = terrain_height_normal.first - (centre_.y - radius);

	//std::cout << terrain_height_normal.first << "\n";
	//std::cout << penetrationDepth << "\n";
	if (penetrationDepth > 0.0f)
	{
		Collision collision;
		collision.SetColliders(this->attachedCollider_, other->GetAttachedCollider());
		collision.penetrationDepth = penetrationDepth + glm::epsilon<float>();
		collision.contact.normal = terrain_height_normal.second;
		collision.contact.point = { centre_.x, terrain_height_normal.first, centre_.z };

		//std::cout << glm::to_string(collision->contact.normal) << "\n";

		//collision.SendCollisionEvent();
		AttachCollision(collision);
		return true;
	}

	return false;
}

bool BoundingSphere::IsIntersecting(const AABB* other) const
{	
	return other->IsIntersecting(this);
	// TODO Fix this.

    const float d1 = fabs(other->GetPosition().y - centre_.y);
    const float d2 = fabs(other->GetPosition().x - centre_.x);
    const float d3 = fabs(other->GetPosition().z - centre_.z);

	const float d1diff = other->size.y * 0.5f + radius - d1;
	const float d2diff = other->size.x * 0.5f + radius - d2;
	const float d3diff = other->size.z * 0.5f + radius - d3;
    //return d1 <= bv->size.y * 0.5f + radius && d2 <= bv->size.x * 0.5f + radius;
	if (d2diff >= 0 && d1diff >= 0 && d3diff >= 0)
	{
		const float width = other->size.x * 0.5f;
		const float height = other->size.y * 0.5f;
		const float depth = other->size.z * 0.5f;
		const float nearest_point_x = glm::clamp(centre_.x, other->GetPosition().x - width,  other->GetPosition().x + width);
		const float nearest_point_y = glm::clamp(centre_.y, other->GetPosition().y - height, other->GetPosition().y + height);
		const float nearest_point_z = glm::clamp(centre_.z, other->GetPosition().z - depth, other->GetPosition().z + depth);

		Collision collisionData;
		collisionData.contact.point = glm::vec3(nearest_point_x, nearest_point_y, nearest_point_z);
		if (collisionData.contact.point == centre_)
			return false;
		collisionData.contact.normal = glm::normalize(centre_ - collisionData.contact.point);
		collisionData.penetrationDepth = fabs(radius - glm::distance(centre_, collisionData.contact.point));
		collisionData.SetColliders(GetAttachedCollider(), other->GetAttachedCollider());
		AttachCollision(collisionData);

		return true;
	}
	return false;
}

bool BoundingSphere::IsIntersecting(const BoundingSphere* other) const
{
	float totalRadius = (other->radius + radius);
	//float totalRadiusSquared = totalRadius * totalRadius;
	float distance = glm::length2(other->centre_ - centre_);

	//float diff = totalRadiusSquared - distance;
	//float dist = radius + other->radius;

	if (distance < totalRadius * totalRadius)
	{
		Collision collision;
		collision.contact.point = centre_ + (other->centre_ - centre_) * (radius / (radius + totalRadius));
		collision.contact.normal = glm::normalize(this->centre_ - other->centre_);
		collision.SetColliders(this->attachedCollider_, other->GetAttachedCollider());
		if (glm::dot(collision.contact.normal, other->centre_ - this->centre_) > 0)
			collision.SetColliders(other->attachedCollider_, this->attachedCollider_);
		collision.penetrationDepth = totalRadius - glm::length(centre_ - other->centre_);

		AttachCollision(collision);
		return true;

	}

	return false;
}

bool BoundingSphere::IsIntersecting(const RayVolume* other) const
{
	glm::vec3 m = other->GetPosition() - this->centre_;
	float b = glm::dot(m, other->direction);

	// Distance
	float c = glm::dot(m, m) - radius * radius;

	if (c > glm::epsilon<float>() && b > glm::epsilon<float>())
		return false;

	float discriminant = b * b - c;
	if (discriminant < glm::epsilon<float>())
		return false;

	// Probably will not have any collision data for ray intersection.
	return true;
}
