#include "BoundingCircle.h"
#include <iostream>
#include "Utils/Util.h"
#include "AABB2D.h"
#include "Physics/Collision2D.h"
#include "Scene/GameObject.h"
#include "OBB2D.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "MyMath.h"
#include "RigidBody2D.h"

bool BoundingCircle::IsIntersecting(const BoundingVolume2D* bv) const
{
	return bv->IsIntersecting(this);
}

bool IsPointInPolygon(const OBB2D* poly, const glm::vec2& point)
{
	float minX = poly->vertices.front().x;
	float maxX = poly->vertices.front().x;
	float minY = poly->vertices.front().y;
	float maxY = poly->vertices.front().y;

	for (int i = 1; i < poly->vertices.size(); ++i)
	{
		minX = Math::Min(poly->vertices[i].x, minX);
		maxX = Math::Max(poly->vertices[i].x, maxX);
		minY = Math::Min(poly->vertices[i].y, minY);
		maxY = Math::Max(poly->vertices[i].y, maxY);
	}

	if (point.x < minX || point.x > maxX || point.y < minY || point.y > maxY)
	{
		return false;
	}

	bool inside = false;
	for ( int i = 0, j = poly->vertices.size() - 1; i < poly->vertices.size(); j = i++)
    {
        if ( ( poly->vertices[ i ].y > point.y ) != ( poly->vertices[ j ].y > point.y ) &&
             point.x < ( poly->vertices[ j ].x - poly->vertices[ i ].x ) * ( point.y - poly->vertices[ i ].y ) / ( poly->vertices[ j ].y - poly->vertices[ i ].y ) + poly->vertices[ i ].x )
        {
            inside = !inside;
        }
    }

    return inside;
}

bool BoundingCircle::IsIntersecting(const OBB2D* bv) const
{
	Collision2D collisionData;
	collisionData.penetrationDepth = 0;
	for (int v = 0; v < bv->vertices.size(); ++v)
	{
		const auto& vert1 = bv->vertices[v != bv->axes.size() - 1 ? v + 1 : 0];
		const auto& vert2 = bv->vertices[v];

		//std::cout << glm::to_string(vert1) << std::endl;

		const auto& line_collision = IsIntersecting(vert1, vert2);

		if (line_collision.first)
		{
			if (line_collision.second.penetrationDepth > collisionData.penetrationDepth)
			{
				collisionData = line_collision.second;
			}
		}

		
		const auto& point_collision = CircleToPoint(vert1);
		if (point_collision.first)
		{
			if (point_collision.second.penetrationDepth > collisionData.penetrationDepth)
			{
				collisionData = point_collision.second;
			}
		}
	}


	if (collisionData.penetrationDepth <= Math::EPSILON)
		return false;

	const auto& val = glm::dot(glm::vec3((centre - bv->centre), 0), (collisionData.contact.normal));
	if (val > 0)
		collisionData.SetColliders(attachedToCollider_, bv->GetAttachedCollider());
	else
		collisionData.SetColliders(bv->GetAttachedCollider(), attachedToCollider_);

		AttachCollision(collisionData);
		//if (_attachedToCollider->unhandledCollisions.find(bv->GetAttachedCollider()) == _attachedToCollider->unhandledCollisions.end())
		//	_attachedToCollider->unhandledCollisions[_attachedToCollider] = collisionData;
	return true;
}

bool BoundingCircle::IsIntersecting(const AABB2D* bv) const
{
    const float d1 = fabs(bv->centre.y - centre.y);
    const float d2 = fabs(bv->centre.x - centre.x);

	const float d1diff = bv->size.y * 0.5f + radius - d1;
	const float d2diff = bv->size.x * 0.5f + radius - d2;
    //return d1 <= bv->size.y * 0.5f + radius && d2 <= bv->size.x * 0.5f + radius;
	if (d2diff >= 0 && d1diff >= 0)
	{
		const float width = bv->size.x * 0.5f;
		const float height = bv->size.y * 0.5f;
		const float nearest_point_x = Math::Clamp(centre.x, bv->centre.x - width, bv->centre.x + width);
		const float nearest_point_y = Math::Clamp(centre.y, bv->centre.y - height, bv->centre.y + height);

		Collision2D collisionData;
		const auto& vec3_centre = glm::vec3(centre, 0);
		collisionData.contact.point = glm::vec3(nearest_point_x, nearest_point_y, 0);
		if (collisionData.contact.point == vec3_centre)
			return false;
		collisionData.contact.normal = glm::normalize(vec3_centre - collisionData.contact.point);
		collisionData.penetrationDepth = radius - glm::length(vec3_centre - collisionData.contact.point);
		collisionData.SetColliders(GetAttachedCollider(), bv->GetAttachedCollider());

		AttachCollision(collisionData);
		//if (_attachedToCollider->unhandledCollisions.find(bv->GetAttachedCollider()) == _attachedToCollider->unhandledCollisions.end())
		//	_attachedToCollider->unhandledCollisions[_attachedToCollider] = collisionData;

		return true;
	}
	return false;
}

bool BoundingCircle::IsIntersecting(const BoundingCircle* bv) const
{
    const float dist = radius + bv->radius;
    if (glm::distance2(centre,bv->centre) <= dist * dist)
    {
		Collision2D collisionData;
		glm::vec3 vec3_centre = glm::vec3(this->centre, 0);
		collisionData.contact.point = vec3_centre + (glm::vec3(bv->centre, 0) - vec3_centre) * (radius / (radius + dist));
		collisionData.contact.normal = glm::normalize(vec3_centre - collisionData.contact.point);
		//collisionData.contact.point = bv->radius * collisionData.contact.normal;
		collisionData.penetrationDepth = dist - glm::length(vec3_centre - glm::vec3(bv->centre, 0)) + glm::epsilon<float>();
		collisionData.SetColliders(attachedToCollider_, bv->GetAttachedCollider());
		//static int counter = 0;
		//counter++;
		//if (counter > 2)
		//std::cout << "Intersect.\n";
		AttachCollision(collisionData);
		//if (_attachedToCollider->unhandledCollisions.find(bv->GetAttachedCollider()) == _attachedToCollider->unhandledCollisions.end())
		//	_attachedToCollider->unhandledCollisions[_attachedToCollider] = collisionData;

		return true;
    }
	return false;
}

BoundingCircle::BoundingCircle()
{
}

BoundingCircle::BoundingCircle(Collider2D* attached, const glm::vec2& centre, float radius)
	: BoundingVolume2D(attached, centre, glm::vec2(radius, radius))
	, radius(radius)
{
}

Projection BoundingCircle::ProjectVertices(const glm::vec2& axis) const
{
	const float temp = glm::dot(axis, centre);
	const float min = temp - radius;
	const float max = temp + radius;
	return { min, max };
}

std::pair<bool, Collision2D> BoundingCircle::IsIntersecting(const glm::vec2& start, const glm::vec2& end) const
{
	const auto& axis = glm::normalize(end - start);
	const auto& projection = glm::dot((centre - start), (axis));
	//std::cout << projection * projection - glm::distance2(end, start) << std::endl;
	if (projection > 0 && projection * projection < glm::distance2(end, start))
	{
		Collision2D collisionData;
		collisionData.penetrationDepth = radius - glm::length(centre - start - projection * axis);
		if (collisionData.penetrationDepth < Math::EPSILON)
			return { false, Collision2D() };
		const auto& perpendicular = Util::PerpendicularClockwise(axis);
		const auto& val = glm::dot((centre - start), (perpendicular));
		collisionData.contact.normal = glm::vec3(perpendicular, 0) * static_cast<float>(((0 < val) - (val < 0)));
		collisionData.contact.point = glm::vec3(axis * projection + start, 0);
		return { true, collisionData };
	}
	return { false, Collision2D()};
}

std::pair<bool, Collision2D> BoundingCircle::CircleToPoint(const glm::vec2& point) const
{
	if (glm::distance2(point, centre) < radius * radius)
	{
		Collision2D collisionData;
		collisionData.penetrationDepth = radius - glm::distance(point, centre);
		collisionData.contact.normal = glm::normalize(glm::vec3(centre - point, 0));
		collisionData.contact.point = glm::vec3(centre, 0) + collisionData.contact.normal * radius;
		return { true, collisionData };
	}
	return { false, Collision2D() };
}


