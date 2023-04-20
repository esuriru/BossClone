#include "OBB2D.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "Utils/Util.h"
#include "MyMath.h"
#include "Collider2D.h"
#include "Physics/Collision2D.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "AABB2D.h"
#include "BoundingCircle.h"
#include "RigidBody2D.h"

bool OBB2D::IsIntersecting(const BoundingVolume2D* bv) const
{
	return bv->IsIntersecting(this);
}

OBB2D::OBB2D()
	: BoundingVolume2D()
{
}

OBB2D::OBB2D(Collider2D* attached, const glm::vec2& centre, const glm::vec2& size)
	: BoundingVolume2D(attached, centre, size)
{
}

OBB2D::OBB2D(Collider2D* attached, const std::vector<glm::vec2>& vertices)
	: BoundingVolume2D(attached, glm::vec2(), glm::vec2())
	, vertices(vertices)
{
	originalVertices = vertices;
}


OBB2D::OBB2D(const AABB2D* to_convert)
	: BoundingVolume2D(to_convert->GetAttachedCollider(), to_convert->centre, to_convert->size)
{
	const glm::vec2& width { to_convert->size.x * 0.5f, 0 };
	const glm::vec2& height{ 0, to_convert->size.y * 0.5f };

	vertices.push_back(to_convert->centre - width - height);
	vertices.push_back(to_convert->centre + width - height);
	vertices.push_back(to_convert->centre + width + height);
	vertices.push_back(to_convert->centre - width + height);

	axes.push_back(glm::normalize(vertices[1] - vertices[0]));
	axes.push_back(glm::normalize(vertices[2] - vertices[1]));
	axes.push_back(glm::normalize(vertices[3] - vertices[2]));
	axes.push_back(glm::normalize(vertices[0] - vertices[3]));


	originalVertices = vertices;
	for (auto& i : originalVertices)
	{
		i -= to_convert->centre;
	}
	originalAxes = axes;
}

void OBB2D::RotateVertices(float clockwiseRotationDegrees)
{
	//glm::vec2 centreBefore = centre;
	Reset();

	//float beforeChange = previousRotation;
	//previousRotation = clockwiseRotationDegrees;

		

	// Rotation is in the Z axis, since it's 2D
	if (clockwiseRotationDegrees > Math::EPSILON)
	{
		glm::quat quat = glm::angleAxis(glm::radians(clockwiseRotationDegrees), glm::vec3(0, 0, 1));

		for (auto& v : vertices)
		{
			v = glm::vec3(quat * glm::vec4(v, 0, 1));
			v += centre;
		}
	}


	for (int v = 0; v < vertices.size(); ++v)
	{
		axes[v] = Util::PerpendicularClockwise(vertices[v + 1 != vertices.size() ? v + 1 : 0] - vertices[v]);
	}

}

void OBB2D::Rotate(const glm::quat& rot)
{
	orientation *= rot;
	for (auto& v : vertices)
	{
		v -= centre;
		v = glm::vec3(rot * glm::vec4(v, 0, 1));
		v += centre;
	}

	for (int v = 0; v < vertices.size(); ++v)
	{
		axes[v] = Util::PerpendicularClockwise(vertices[v + 1 != vertices.size() ? v + 1 : 0] - vertices[v]);
	}
}

void OBB2D::Reset()
{
	vertices = originalVertices;
	axes = originalAxes;
}

OBB2D* OBB2D::CreateRectangleOBB(Collider2D* attached, const glm::vec2& centre, const glm::vec2& size)
{
	OBB2D* temp = new OBB2D(attached, centre, size);
	const glm::vec2& width { size.x * 0.5f, 0};
	const glm::vec2& height{ 0, size.y * 0.5f };

	temp->attachedToCollider_ = attached;

	temp->vertices.push_back(centre - width - height);
	temp->vertices.push_back(centre + width - height);
	temp->vertices.push_back(centre + width + height);
	temp->vertices.push_back(centre - width + height);

	//temp->axes.push_back((temp->vertices[1] - temp->vertices[0]).PerpendicularClockwise());
	//temp->axes.push_back((temp->vertices[2] - temp->vertices[1]).PerpendicularClockwise());
	//temp->axes.push_back((temp->vertices[3] - temp->vertices[2]).PerpendicularClockwise());
	//temp->axes.push_back((temp->vertices[0] - temp->vertices[3]).PerpendicularClockwise());

	temp->axes.push_back(glm::normalize(temp->vertices[1] - temp->vertices[0]));
	temp->axes.push_back(glm::normalize(temp->vertices[2] - temp->vertices[1]));
	temp->axes.push_back(glm::normalize(temp->vertices[3] - temp->vertices[2]));
	temp->axes.push_back(glm::normalize(temp->vertices[0] - temp->vertices[3]));
	temp->originalVertices = temp->vertices;
	temp->originalAxes = temp->axes;

	for (auto& i : temp->originalVertices)
	{
		i -= centre;
	}

	return temp;
}

OBB2D* OBB2D::CreateTriangleOBB(Collider2D* attached, const glm::vec2& centre, const glm::vec2& size)
{
	OBB2D* temp = new OBB2D(attached, centre, size);
	temp->attachedToCollider_ = attached;
	const float half_length = 0.5f;
	temp->vertices.push_back({ -half_length, -half_length });
	temp->vertices.push_back({half_length, -half_length});
	temp->vertices.push_back({0, half_length});
	for (auto& v: temp->vertices)
	{
		v.x *= size.x;
		v.y *= size.y;
		v += centre;
	}

	temp->axes.push_back(Util::PerpendicularClockwise(temp->vertices[1] - temp->vertices[0]));
	temp->axes.push_back(Util::PerpendicularClockwise(temp->vertices[2] - temp->vertices[1]));
	temp->axes.push_back(Util::PerpendicularClockwise(temp->vertices[0] - temp->vertices[2]));
	
	temp->originalVertices = temp->vertices;
	temp->originalAxes = temp->axes;
	for (auto& i : temp->originalVertices)
	{
		i -= centre;
	}
	return temp;
}

bool OBB2D::IsIntersecting(const OBB2D* bv) const
{
	// Might just make FindMinOverlapAxis return an axis
	const auto& localMinSep = FindMinOverlapAxis(bv);
	const auto& otherMinSep = bv->FindMinOverlapAxis(this);
	//std::cout << "First overlap: " << localMinSep.axis_overlap << " Second overlap: " << otherMinSep.axis_overlap << "\n";
	if (localMinSep.axis_overlap > 0 && otherMinSep.axis_overlap > 0)
	{
		Collision2D collisionData;
		const auto& totalMinSep = Math::Min(localMinSep.axis_overlap, otherMinSep.axis_overlap);
		glm::vec2 penetrationDepthNormal;
		if (totalMinSep == localMinSep.axis_overlap)
			
		{
			penetrationDepthNormal = localMinSep.axis_direction;
			//collisionData.contact.normal = penetrationDepthNormal;
			//collisionData.SetColliders(_attachedToCollider, bv->GetAttachedCollider());
		}
		else
		{
			penetrationDepthNormal = otherMinSep.axis_direction;
			//collisionData.contact.normal = penetrationDepthNormal;
		}
		const auto& val = glm::dot((centre - bv->centre), (penetrationDepthNormal));
		collisionData.contact.normal = glm::vec3(penetrationDepthNormal * static_cast<float>(((0 < val) - (val < 0))), 0);

		float minDistSq = std::numeric_limits<float>::max();
		bool otherContactExists = true;
		glm::vec2 otherContact{};

		for (int i = 0; i < vertices.size(); ++i)
		{
			glm::vec2 p = vertices[i];

			for (int j = 0; j < bv->vertices.size(); ++j)
			{
				glm::vec2 va = bv->vertices[j];
				glm::vec2 vb = bv->vertices[(j + 1) % bv->vertices.size()];

				const auto& result = PointSegmentDistance(p, va, vb);

				if (fabs(result.first - minDistSq) < glm::epsilon<float>() * glm::epsilon<float>())
				{
					otherContactExists = true;
					otherContact = result.second;
				}
				else if (result.first < minDistSq)
				{
					minDistSq = result.first;
					collisionData.contact.point = glm::vec3(result.second, 0);
				}
			}
		}

		for (int i = 0; i < bv->vertices.size(); ++i)
		{
			glm::vec2 p = bv->vertices[i];

			for (int j = 0; j < vertices.size(); ++j)
			{
				glm::vec2 va = vertices[j];
				glm::vec2 vb = vertices[(j + 1) % vertices.size()];

				const auto& result = PointSegmentDistance(p, va, vb);

				if (fabs(result.first - minDistSq) < glm::epsilon<float>() * glm::epsilon<float>())
				{
					otherContactExists = true;
					otherContact = result.second;
				}
				else if (result.first < minDistSq)
				{
					minDistSq = result.first;
					collisionData.contact.point = glm::vec3(result.second, 0);
				}
			}
		}

		if (otherContactExists)
			collisionData.contact.point = (glm::vec3(otherContact, 0) + collisionData.contact.point) * 0.5f;
			//collisionData.contact.normal = penetrationDepthNormal;
		collisionData.SetColliders(attachedToCollider_, bv->GetAttachedCollider());

		collisionData.penetrationDepth = totalMinSep;

		//std::cout << glm::to_string(collisionData.contact.point) << "\n";
		//if (_attachedToCollider->unhandledCollisions.find(bv->GetAttachedCollider()) == _attachedToCollider->unhandledCollisions.end())
		//	_attachedToCollider->unhandledCollisions[_attachedToCollider] = collisionData;
		AttachCollision(collisionData);

		return true;
	}
	return false;
}

bool OBB2D::IsIntersecting(const AABB2D* bv) const
{
	AABB2D temp = *bv;
	OBB2D tempOBB = OBB2D(&temp);
	
	return IsIntersecting(&tempOBB);
}

bool OBB2D::IsIntersecting(const BoundingCircle* bv) const
{
	return bv->IsIntersecting(this);
}

void OBB2D::SetPosition(const glm::vec2& pos)
{
	SetPosition(glm::vec3(pos, 0));
}

void OBB2D::SetOrientation(float degrees)
{
	//orientation = degrees;
	RotateVertices(degrees);
}

void OBB2D::SetPosition(const glm::vec3& pos)
{
	const auto& trans = glm::vec2(pos) - centre;
	for (auto& v : vertices)
	{
		v += trans;
	}
	centre = pos;
	for (int v = 0; v < vertices.size(); ++v)
	{
		axes[v] = Util::PerpendicularClockwise(vertices[v != vertices.size() - 1 ? v + 1 : 0] - vertices[v]);
	}

}

Projection OBB2D::ProjectVertices(const glm::vec2& axis) const
{
	float min = glm::dot(axis, vertices[0]);
	float max = min;
	for (int i = 1; i < vertices.size(); ++i)
	{
		float t = glm::dot(axis, (vertices[i]));
		if (t < min)
		{
			min = t;
		}
		else if (t > max)
		{
			max = t;
		}
	}
	return { min, max };
}

std::pair<float, glm::vec2> OBB2D::PointSegmentDistance(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) const
{
	glm::vec2 ab = b - a;
	glm::vec2 ap = p - a;

	float proj = glm::dot(ap, ab);
	float abLenSq = glm::length2(ab);
	float d = proj / abLenSq;

	glm::vec2 cp;

	if(d <= 0.f)
	{
		cp = a;
	}
	else if(d >= 1.f)
	{
		cp = b;
	}
	else
	{
		cp = a + ab * d;
	}

	return { glm::distance2(p, cp), cp };
}

OBB2D::Axis OBB2D::FindMinOverlapAxis(const OBB2D* bv) const
{
	float overlap = std::numeric_limits<float>::max();
	Axis minSepAxis = { {0, 0}, 0 };
	//for (int vertice = 0; vertice < vertices.size(); ++vertice)
	for (int a = 0; a < axes.size(); ++a)
	{
		//const glm::vec2 normal = ((vertice != vertices.size() - 1 ? vertices[vertice + 1] : vertices[0]) - vertices[vertice]).PerpendicularClockwise();
		const glm::vec2& normal = axes[a];

		Projection p1 = ProjectVertices(normal);
		Projection p2 = bv->ProjectVertices(normal);

		if (!p1.Overlap(p2))
		{
			return { {0, 0} , 0 };
		}
		const float o = p1.GetOverlap(p2);
		if (o < overlap)
		{
			overlap = o;
			minSepAxis.axis_direction = normal;
			minSepAxis.axis_overlap = overlap;
		}
	}
	
	return minSepAxis;
}
