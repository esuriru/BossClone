#include "AABB.h"
#include <glm/gtx/intersect.hpp>
#include <array>
#include "RigidBody.h"
#include <limits>
#include "Scene/GameObject.h"
#include "Transform.h"
#include "RigidBody.h"
#include "TerrainVolume.h"
#include "RayVolume.h"
#include <glm/gtx/norm.hpp>
#include "TerrainVolume.h"
#include "BoundingSphere.h";
#include "Physics/Collision.h"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/constants.hpp>
#include "Core/Log.h"

AABB::AABB(Collider* parent, glm::vec3 centre, glm::vec3 size)
	: BoundingVolume(parent), size(size)
{
	AABB::SetPosition(centre);
}

bool AABB::IsIntersecting(const TerrainVolume* other) const
{
	const auto terrain_height_normal = other->GetHeightAndNormal(centre_.x, centre_.z);
	auto penetrationDepth = terrain_height_normal.first - (centre_.y -  (size.y) * 0.5f);

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

bool AABB::IsIntersecting(const AABB* other) const
{
	constexpr glm::vec3 faces[6] =     
	{
		glm::vec3(-1,  0,  0), // 'left' face normal (-x direction)
		glm::vec3( 1,  0,  0), // 'right' face normal (+x direction)       
		glm::vec3( 0, -1,  0), // 'bottom' face normal (-y direction)        
		glm::vec3( 0,  1,  0), // 'top' face normal (+y direction)        
		glm::vec3( 0,  0, -1), // 'far' face normal (-z direction)        
		glm::vec3( 0,  0,  1) // 'near' face normal (+x direction)    
	};        
	std::array<float, 6> distances = 
	{         
		(other->max_.x - min_.x),
		(max_.x - other->min_.x), // distance of box 'b' to face on 'right' side of 'a'.        
		(other->max_.y - min_.y), // distance of box 'b' to face on 'bottom' side of 'a'.        
		(max_.y - other->min_.y), // distance of box 'b' to face on 'top' side of 'a'.        
		(other->max_.z - min_.z), // distance of box 'b' to face on 'far' side of 'a'.       
		(max_.z - other->min_.z) // distance of box 'b' to face on 'near' side of 'a'.     
	};    
	float lowest_distance = 0;
	glm::vec3 normal = faces[0];
	int contact_face = 0;

	for (int i = 0; i < 6; ++i)    
	{        
		// box does not intersect face. So boxes don't intersect at all.        
		if(distances[i] < 0.0f)
			return false;

		// face of least intersection depth. That's our candidate.        
		if((i == 0) || distances[i] < lowest_distance)
		{            
			contact_face = i;
			normal = faces[i];
			lowest_distance = distances[i];
		}            
	} 

	Collision collision;
	collision.contact.normal = normal;
	if (glm::dot(centre_ - other->centre_, collision.contact.normal) > 0)
		collision.SetColliders(this->attachedCollider_, other->attachedCollider_);
	else
		collision.SetColliders(other->attachedCollider_, this->attachedCollider_);
	collision.penetrationDepth = lowest_distance;

	AttachCollision(collision);

	return true;
}

bool AABB::IsIntersecting(const BoundingSphere* other) const
{
	constexpr float epsilon_squared = glm::epsilon<float>() * glm::epsilon<float>();
	glm::vec3 closest_point =
		glm::clamp(other->GetPosition(), min_, max_);

	float dist = glm::distance2(closest_point, other->GetPosition());
	if (dist <= other->radius * other->radius && dist > epsilon_squared)
	{
		Collision collision;
		collision.SetColliders(other->GetAttachedCollider(), attachedCollider_);
		collision.contact.point = closest_point;
		collision.contact.normal = glm::normalize(other->GetPosition() - closest_point);
		collision.penetrationDepth = fabs(other->radius - glm::distance(other->GetPosition(), closest_point));

		AttachCollision(collision);
		return true;
	}
	return false;
}


bool AABB::IsIntersecting(const RayVolume* other) const
{
	glm::vec3 invDir = 1.0f / other->direction;
	glm::vec3 Min = (min_ - other->GetPosition()) * invDir;
	glm::vec3 Max = (max_ - other->GetPosition()) * invDir;

	glm::vec3 t1 = glm::min(Min, Max);
	glm::vec3 t2 = glm::max(Min, Max);

	float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
	float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);
   	if (tNear > tFar || tFar < 0.0f)
		return false;
	return true;
}


void AABB::SetPosition(const glm::vec3& pos)
{
	centre_ = pos;
	min_ = centre_ - 0.5f * size;
	max_ = centre_ + 0.5f * size;
}

void AABB::SetSize(const glm::vec3& size)
{
	this->size = size;
	min_ = centre_ - 0.5f * size;
	max_ = centre_ + 0.5f * size;
}

