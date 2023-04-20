#include "RandomDroppable.h"
#include "MyMath.h"
#include "Scene/GameObject.h"
#include "Transform.h"
#include "KeyboardController.h"
#include "Utils/Input.h"
#include "MeshFilter.h"
#include "RigidBody2D.h"
#include "Collider2D.h"
#include "Physics/ColliderManager.h"

constexpr float right_limit = (4.f / 3.f) * 500.f - 60.f;

RandomDroppable::RandomDroppable(GameObject* go)
	: Droppable(go)
	, lowerLimit(60.f)
	, upperLimit(right_limit)
{
	lifetimeTimer_ = Math::RandFloatMinMax(5.f, 7.5f);
}

void RandomDroppable::UpdateGravity()
{
	if (isDirty_)
	{
		gameObject_->GetTransform()->SetPosition(glm::vec3(
			Math::RandFloatMinMax(lowerLimit, upperLimit),
			427.5f,
			0.f
		));
	}
}

void RandomDroppable::FixedUpdate()
{
	float dt = static_cast<float>(FIXED_UPDATE_FRAME_TIME);
	if (isDirty_)
	{
		mf_ = gameObject_->GetComponent<MeshFilter>();
		rb_ = gameObject_->GetComponent<RigidBody2D>();

		if (mf_)
		{
			mat_ = new Material({ 1.0f, 1.0f, 1.0f, 1.0f }, Shader::DefaultShader);
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
	static KeyboardController* kc = KeyboardController::GetInstance();
	if (!isDropped_)
	{
		const auto& mouseCoords = input->GetMouseCoords();
		gameObject_->GetTransform()->SetPosition(glm::vec3(
			Math::RandFloatMinMax(lowerLimit, upperLimit),
			420.f,
			0.f
		));

		isDropped_ = true;
		if (rb_)
		{
			rb_->SetActive(true);
			rb_->GetAttachedCollider()->SetActive(true);
			rb_->Reset();
			ColliderManager::GetInstance()->Update(dt);
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
		}
	}

}
