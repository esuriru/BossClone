#include "Droppable.h"
#include "KeyboardController.h"
#include "Physics/ColliderManager.h"
#include "MeshFilter.h"
#include "Scene/GameObject.h"
#include "Transform.h"
#include "RigidBody2D.h"
#include <glm/glm.hpp>
#include "MyMath.h"
#include "Utils/Input.h"
#include "Core/Application.h"

Droppable::Droppable(GameObject* go)
	: Component(go)
	, isDropped_(false)
	, mf_(go->GetComponent<MeshFilter>())
	, rb_(go->GetComponent<RigidBody2D>())
	, lifetime_(10.f)
{
	gameObject_->GetTransform()->SetEulerAngles({ 0,0,0 });
}

Droppable::~Droppable()
{
	delete mat_;
}

void Droppable::Update(double dt)
{
}

void Droppable::FixedUpdate()
{
	float dt = static_cast<float>(FIXED_UPDATE_FRAME_TIME);
	if (isDirty_)
	{
		mf_ = gameObject_->GetComponent<MeshFilter>();
		rb_ = gameObject_->GetComponent<RigidBody2D>();

		if (mf_)
		{
			mat_ = new Material({ 1.0f, 1.0f, 1.0f, 0.5f }, Shader::DefaultShader);
			mf_->GetMesh()->materials.clear();
			mf_->GetMesh()->materials.push_back(mat_);
		}
		if (rb_)
		{
			rb_->SetActive(false);
			rb_->GetAttachedCollider()->SetActive(false);
			rb_->Reset();
		}
		isDirty_ = false;
	}

	static Input* input = Input::GetInstance();
	//static KeyboardController* kc = KeyboardController::GetInstance();
	constexpr float right_limit = (4.f / 3.f) * 500.f - 60.f;
	if (!isDropped_)
	{
		const auto& mouseCoords = input->GetMouseCoords();
		gameObject_->GetTransform()->SetPosition(glm::vec3(
			Math::Clamp(mouseCoords.x, 60.f, right_limit),
			475.f,
			0.f
		));

		if (input->GetMouseButtonDown(0))
		{
			isDropped_ = true;
			if (rb_)
			{
				rb_->SetActive(true);
				rb_->GetAttachedCollider()->SetActive(true);
				rb_->Reset();
				ColliderManager::GetInstance()->Update(dt);
			}
			if (mf_)
				mf_->GetMesh()->materials.front()->data->_diffuse.a = 1.0f;
		}
	}
	if (isDropped_)
	{
		lifetimeTimer_ += dt;
		if (lifetimeTimer_ >= lifetime_ || gameObject_->GetTransform()->GetPosition().y < -80.f ||
			gameObject_->GetTransform()->GetPosition().x < -50.f ||gameObject_->GetTransform()->GetPosition().x > right_limit + 120.f)
		{
			isDropped_ = false;
			gameObject_->GetTransform()->SetEulerAngles({ 0, 0, 0 });
			lifetimeTimer_ = 0;
			if (rb_)
			{
				rb_->SetActive(false);
				rb_->GetAttachedCollider()->SetActive(false);
			}
			if (mf_)
				mf_->GetMesh()->materials.front()->data->_diffuse.a = 0.5f;
		}
	}

}

bool Droppable::IsDropping() const
{
	return isDropped_;
}
