#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/common.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>

constexpr float EPSILON_SQUARED = glm::epsilon<float>() * glm::epsilon<float>();
constexpr glm::mat4 identity = glm::mat4(1.0f);

void Transform::CalculateTransformMatrix()
{
	localMatrix_ = identity;

	localMatrix_ = glm::translate(localMatrix_, position_);

	if (length2(rotation_) > EPSILON_SQUARED)
	{
		localMatrix_ = localMatrix_ * glm::mat4_cast(rotation_);
	}

	localMatrix_ = glm::scale(localMatrix_, scale_);

	worldMatrix_ = (parent_) ? parent_->worldMatrix_ * localMatrix_ : localMatrix_;
	
	for (auto& i : children_)
	{
		i->CalculateTransformMatrix();
	}
}

void Transform::ResetDirectionVectors()
{
	up = { 0, 1, 0 };
	right = { 1, 0, 0 };
	forward = { 0, 0, -1 };
}

void Transform::UpdateDirectionVectors()
{
	up = rotation_ * up;
	right = rotation_ * right;
	forward = rotation_ * forward;
}

void Transform::ChangeLocalToWorld(Transform* refWorld)
{
	//const auto& inverseWorldMatrix = glm::inverse(refWorld->_worldMatrix);
	glm::vec3 worldScale;
	glm::quat worldOrientation;
	glm::vec3 worldTranslation;
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(worldMatrix_, worldScale, worldOrientation, worldTranslation, skew, perspective);


#if 0
	_position = inverseWorldMatrix * glm::vec4(_position, 1.0f);
	_rotation *= glm::quat_cast(inverseWorldMatrix);
	_scale = inverseWorldMatrix * glm::vec4(_scale, 1.0f);
#endif
	position_ = worldTranslation;
	rotation_ = worldOrientation;
	scale_ = worldScale;

	isDirty_ = true;
}

Transform::Transform(GameObject* go)
	: 
	Component(go),
	position_(0, 0, 0),
	rotation_(glm::quat({ 0, 0, 0 })),
	scale_(1, 1, 1),
	parent_(nullptr),
	localMatrix_(1.0f),
	worldMatrix_(1.0f),
	up(0, 1, 0),
	right(1, 0, 0),
	forward(0, 0, -1)
{
}

glm::vec3 Transform::GetFront()
{
	if (isDirty_)
	{
		CalculateTransformMatrix();
		isDirty_ = false;
	}
	
	return glm::mat4(glm::mat3(worldMatrix_)) * glm::vec4(forward, 0.0f);
}

void Transform::SetParent(Transform* parent)
{
	if (!parent && parent_)
	{
		parent_->children_.erase(parent_->children_.begin() + childIndex_);
		ChangeLocalToWorld(parent_);
		parent_ = parent;
		childIndex_ = 0;
	}
	else if (parent)
	{
		parent_ = parent;
		this->childIndex_ = parent->children_.size();
		parent->children_.push_back(this);
		parent_->CalculateTransformMatrix();
		parent_->SetDirty(false);
	}
	else
		return;

	isDirty_ = true;
}

Transform* Transform::GetParent() const
{
	return parent_;
}

Transform* Transform::GetChild(uint32_t index) const
{
	if (children_.empty() || children_.size() - 1 < index)
		return nullptr;
	return children_[index];
}

void Transform::Translate(const glm::vec3& trans)
{
	position_ += trans;
	isDirty_ = true;
}

void Transform::Rotate(const glm::quat& rot)
{
	rotation_ *= rot;
	UpdateDirectionVectors();

	isDirty_ = true;
}

void Transform::SetPosition(const glm::vec3& pos)
{
	position_ = pos;
	isDirty_ = true;
}

void Transform::SetRotation(const glm::quat& rot)
{
	rotation_ = rot;

	ResetDirectionVectors();
	UpdateDirectionVectors();

	isDirty_ = true;
}

void Transform::SetEulerAngles(const glm::vec3& rot)
{
	const auto& rotation = glm::radians(rot);

	// TODO - This is wrong.
	rotation_ = glm::normalize(glm::angleAxis(rotation.z, glm::vec3(0, 0, 1)) * glm::angleAxis(rotation.y, up) * glm::angleAxis(rotation.x, right));
	ResetDirectionVectors();
	UpdateDirectionVectors();

	isDirty_ = true;
}

void Transform::SetScale(const glm::vec3& scl)
{
	scale_ = scl;
	isDirty_ = true;
}

glm::vec3 Transform::InverseTransformPoint(const glm::vec3& point)
{
	return localMatrix_ * glm::vec4(point, 1.0f);
}

glm::vec3 Transform::GetPosition(void) const
{
	return position_;
}

glm::vec3 Transform::GetEulerAngles(void) const
{
	return glm::degrees(glm::eulerAngles(rotation_));
}

const glm::quat& Transform::GetRotation(void) const
{
	return rotation_;
}

glm::vec3 Transform::GetScale(void) const
{
	return scale_;
}

glm::vec3 Transform::GetWorldPosition() 
{
	if (!parent_)
		return position_;

	if (isDirty_)
	{
		CalculateTransformMatrix();
		isDirty_ = false;
	}
	
	return worldMatrix_ * glm::vec4(position_, 1.0f);
}

glm::mat4 Transform::GetTransformMatrix()
{
	if (isDirty_)
	{
		CalculateTransformMatrix();
		isDirty_ = false;
	}
	return worldMatrix_;
}

void Transform::ForceUpdate()
{
	CalculateTransformMatrix();
}

void Transform::Follow(Transform* other)
{
	position_ = other->position_;
	//_rotation = other->_rotation;
	isDirty_ = true;
}

void Transform::Update(double dt)
{
}


