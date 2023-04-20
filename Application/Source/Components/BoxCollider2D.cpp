#include "BoxCollider2D.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "Scene/GameObject.h"
#include "CircleCollider2D.h"
#include <glm/glm.hpp>
#include "AABB2D.h"
#include "OBB2D.h"

#define DEBUG_LOG(object) if (_DEBUG) std::cout << (object) << "\n";

BoxCollider2D::BoxCollider2D(GameObject* go)
	: Collider2D(go)
	, localRotation_(0)
	, rotated_(false)
{
	normalBox_ = AABB2D(this, targetTransform_->GetPosition(), targetTransform_->GetScale());
	rotatedBox_ = OBB2D(&normalBox_);
	boundingVolume = &normalBox_;
}

void BoxCollider2D::Update(double dt)
{
	//boundingVolume->centre = _position + _offset;
	//boundingVolume->SetPosition(_position + _offset);
	if (!attachedRigidbody)
		position_ = gameObject_->GetTransform()->GetPosition();
}

void BoxCollider2D::UpdateGravity()
{
	if (isDirty_)
	{
		RetrieveRigidBody();
		isDirty_ = false;
	}
	UpdateRotation();
}

bool BoxCollider2D::IsColliding(const Collider2D* c) const
{
	//return c->IsColliding(this);
	return c->boundingVolume->IsIntersecting(this->boundingVolume);
}

void BoxCollider2D::Rotate(float clockwiseRotationDegrees)
{
	if (rotated_)
		rotatedBox_.RotateVertices(clockwiseRotationDegrees);
	else
	{
		if (fabs(clockwiseRotationDegrees) > glm::epsilon<float>())
		{
			rotated_ = true;
			boundingVolume = &rotatedBox_;
			rotatedBox_.RotateVertices(clockwiseRotationDegrees);
		}
	}
}

void BoxCollider2D::Rotate(const glm::quat& rot)
{
	rotatedBox_.Rotate(rot);
}

void BoxCollider2D::SetPosition(const glm::vec2& pos)
{
	position_ = pos;
	boundingVolume->SetPosition(position_ + offset_);
}


void BoxCollider2D::ComputeAxes()
{
	//_axes[0] = _corners[1] - _corners[0];
	//_axes[1] = _corners[3] - _corners[0];

	//for (int a = 0; a < 2; ++a)
	//{
	//	_axes[a] *= 1 / _axes[a].LengthSquared();
	//	_origins[a] = _corners[0].Dot(_axes[a]);
	//}
}


void BoxCollider2D::UpdateRotation()
{
	const auto& transform_rotation = gameObject_->GetTransform()->GetRotation();
	//_rotatedBox.RotateVertices();
	//if (_localRotation != transform_rotation)
	//{
	//	_localRotation = transform_rotation;
	//	const int& rot = static_cast<int>(_localRotation);
	//	
	//	if (rot != 0)
	//	{
	//		// Change into an OBB2D
	//		OBB2D* newVolume = OBB2D::CreateRectangleOBB(this, boundingVolume->centre, boundingVolume->size);
	//		newVolume->RotateVertices(_localRotation);

	//		delete boundingVolume;
	//		boundingVolume = newVolume;
	//	}
	//	else
	//	{
	//		// Convert back to an AABB2D
	//	}
	//	
	//}
}

//glm::vec2 BoxCollider2D::GetSize(void) const
//{
//	//return _size;
//	return glm::vec2();
//}
