#include "FallingObject.h"

#include "Scene/GameObject.h"
#include "MyMath.h"
#include "RigidBody2D.h"
#include "Transform.h"
#include "Renderer/SceneRenderer.h"

FallingObject::FallingObject(GameObject* go)
	: Component(go)
	, transform_(go->GetTransform())
	, referenceCollider_(go->GetComponent<Collider2D>())
	, objectRotation_(0.f)
	, rateOfSpin_(0.f)
	, lifetime(10.f)
{
	ResetPosition();
}

void FallingObject::ResetPosition()
{
	//_rateOfSpin = Math::RandFloatMinMax(0.1f, 0.5f);
	timer_ = 0.f;
	float xValue = Math::RandFloatMinMax(30.f, 620.f);
	if (xValue < 375.f)
		xValue = Math::Clamp(xValue, 30.f, 250.f);
	if (xValue >= 375.f)
		xValue = Math::Clamp(xValue, 580.f, 620.f);
	transform_->SetPosition({ xValue, 530.f, 0.f });
	transform_->SetRotation({ 0, 0, Math::RandFloatMinMax(0.f, 360.f) });
	referenceCollider_->Rotate(transform_->GetRotation().z);
	if (referenceCollider_->attachedRigidbody)
		referenceCollider_->attachedRigidbody->AddForce({ Math::RandFloatMinMax(-30.f, 30.f), 0, 0 }, RigidBody2D::FORCE_MODE::IMPULSE);

	gameObject_->SetActive(true);
}

void FallingObject::Update(double dt)
{
	//SceneRenderer::GetInstance()->RenderTextOnScreen(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEOMETRY_TYPE::GEO_TEXT], "Test", Color(1, 1, 1), 20, 200, 300);
	if (isDirty_)
	{
		referenceCollider_ = gameObject_->GetComponent<Collider2D>();
		isDirty_ = false;
	}
	timer_ += static_cast<float>(dt);
	if (timer_ >= lifetime)
	{
		gameObject_->SetActive(false);
		ResetPosition();
	}
	//_objectRotation += _rateOfSpin;
	//if (_objectRotation >= 360)
	//	_objectRotation -= 360;
	//_transform->SetRotation({ 0, 0, _objectRotation });
}
