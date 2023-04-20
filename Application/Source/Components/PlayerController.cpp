#include "PlayerController.h"
#include "Physics/ColliderManager.h"
#include "MyMath.h"
#include "InteractButton.h"
#include <glm/gtc/quaternion.hpp>
#include "GLFW/glfw3.h"
#include "MeshFilter.h"
#include "Interactable.h"
#include "Camera.h"
#include <glm/gtx/string_cast.hpp>
#include "Transform.h"
#include "KeyboardController.h"
#include "Terrain.h"
#include "Utils/Input.h"
#include "CameraController.h"
#include "Core/Log.h"
#include "SphereCollider.h"
#include "BoxCollider.h"

PlayerController::PlayerController(GameObject* go)
	: RigidBody(go)
	, targetTransform_(go->GetTransform())
	, isAirborne_(true)
	, refTerrain_(nullptr)
	, collisionOffset_({ 0, 3.75f, 0})
	, followingCameraTransform_(go->GetTransform())
	, rightArm_(nullptr)
	, holdingItem_(false)
	, interactable_(nullptr)
	, itemHolding_(nullptr)
{
	ray_ = go->AddComponent<Ray>();
	cameraOffset_ = { 0, -0.5f, 0.0f };
	go->tag = "Player";
	//targetTransform_->SetPosition({ 0, -10, 0 });
	defaultSpeed = 15.0f;
	runSpeed = 30.0f;
	currentSpeed = 15.0f;
	bodyType_ = RIGID_BODY_TYPE::STATIC;
	auto sphereCollider = go->AddComponent<BoxCollider>();
	sphereCollider->SetSize({ 6, 6, 2 });
	targetCollider_ = sphereCollider;
	targetCollider_->attachedRigidbody = this;
}

void PlayerController::SetTerrain(std::shared_ptr<Terrain> t)
{
	refTerrain_ = t;
	if (targetCollider_)
		ColliderManager::GetInstance()->AddBlockage(this->targetCollider_, t->GetComponent<TerrainCollider>());
}

void PlayerController::Update(double dt)
{
	if (isDirty_)
	{
		// Player base -> Player torso -> Right arm pivot
		const auto& torso = targetTransform_->GetChild(0);
		//targetCollider_ = gameObject_->GetComponent<Collider>();

		if (torso)
			rightArm_ = torso->GetChild(2);
		isDirty_ = false;
	}

	if (!refTerrain_)
		return;
	//CC_INFO(glm::to_string(targetTransform_->GetPosition()));
	deltaTime_ = static_cast<float>(dt);
	HandleInteraction();
	HandleKeyPress();

	HandleGravity();
	//HandleTerrainCollision();
}

void PlayerController::HandleKeyPress()
{
	constexpr glm::vec3 BACK = { 0, 0, -1 }; // This is actually front.
	constexpr glm::vec3 UP = { 0, 1, 0 };
	constexpr glm::vec3 RIGHT = { 1, 0, 0 };

	static Input* kc_instance = Input::GetInstance();
	if (/*!_refCamera && */refCamera_->IsActive())
	{
		static Input* input = Input::GetInstance();
		auto offset = input->GetMouseOffset();
		//std::cout << glm::to_string(offset) << "\n";

		offset *= MOUSE_SENSITIVITY;


		// Yaw
		rotation.y -= offset.x;

		if (rotation.y > 360)
			rotation.y -= 360;

		if (rotation.y < -360)
			rotation.y += 360;

		targetTransform_->SetEulerAngles(rotation);
	}


	if (refCrosshair_)
	{
		refCrosshair_->SetActive(refCamera_->IsActive());
	}


	const auto& front = targetTransform_->GetRotation() * BACK;
	const auto& right = targetTransform_->GetRotation() * RIGHT;


	if (kc_instance->IsKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		currentSpeed += (runSpeed - currentSpeed) * deltaTime_ * 2.0f;
		if (currentSpeed > 29.9)
			currentSpeed = runSpeed;
		//CC_INFO(std::to_string(currentSpeed));
	}

	if (kc_instance->IsKeyUp(GLFW_KEY_LEFT_SHIFT))
	{
		// Decrease speed gradually back to the default speed
		currentSpeed += (defaultSpeed - currentSpeed) * deltaTime_ * 2.0f;
		if (currentSpeed < 15.1)
			currentSpeed = defaultSpeed;
		//CC_INFO(std::to_string(currentSpeed));
	}
	//std::cout << glm::to_string(front) << "\n";
	
	if (kc_instance->IsKeyDown('W'))
	{
		targetTransform_->Translate(front * deltaTime_ * currentSpeed);

	}
	if (kc_instance->IsKeyDown('S'))
	{
		targetTransform_->Translate(-front * deltaTime_ * currentSpeed);
	}
	if (kc_instance->IsKeyDown('A'))
	{
		targetTransform_->Translate(-right * deltaTime_ * currentSpeed);
	}
	if (kc_instance->IsKeyDown('D'))
	{
		targetTransform_->Translate(right * deltaTime_ * currentSpeed);
	}
	float jump_speed = 10;
	if (kc_instance->IsKeyDown(VK_SPACE) && !isAirborne_)
	{
		verticalSpeed_ = jump_speed;
		isAirborne_ = true;
	}

	if (refCamera_)
	{
		const auto& calculated_position = cameraOffset_ + followingCameraTransform_->GetPosition();
		refCamera_->GetGameObject()->GetTransform()->SetPosition(calculated_position);

		if (ray_ && refCamera_->IsActive())
		{
			ray_->SetDirection(refCamera_->GetGameObject()->GetTransform()->GetRotation() * BACK);
			ray_->SetPosition(calculated_position);
		}
	}

	//if (isAirborne_)
	//	verticalSpeed_ += -gravity_scale * deltaTime_;

	targetTransform_->ForceUpdate();
}

void PlayerController::HandleTerrainCollision()
{
	if (!targetCollider_)
		return;

	for (auto& i : targetCollider_->unhandledCollisions)
	{
		if (i.second.handled)
			continue;

		if (i.second.rigidbody == this)
			if (i.second.otherRigidbody)
			{
				i.second.rigidbody = nullptr;
				i.second.SendCollisionEvent();
			}
			else
			{
				// Resolve it here.
				targetCollider_->Translate(i.second.contact.normal * i.second.penetrationDepth);
			}
		else if (i.second.otherRigidbody == this)
			if (i.second.rigidbody)
			{
				i.second.otherRigidbody = nullptr;
				i.second.SendCollisionEvent();
			}
			else
			{
				// Resolve it here.
				targetCollider_->Translate(-i.second.contact.normal * i.second.penetrationDepth);
			}
		i.second.handled = true;
	}
	targetCollider_->unhandledCollisions.clear();
}

void PlayerController::HandleGravity()
{
	if (isAirborne_)
		verticalSpeed_ += -gravity_scale * deltaTime_;

	targetTransform_->Translate({ 0, verticalSpeed_ * deltaTime_, 0 });
	targetCollider_->SetPosition(targetTransform_->GetPosition() + collisionOffset_);
	//targetTransform_->Translate({ 0, verticalSpeed_ * deltaTime_, 0 });
	if (refTerrain_)
	{
		constexpr glm::vec3 height_diff = glm::vec3(0, 1.0f, 0);
		const auto& pos = targetTransform_->GetPosition() + height_diff;
		const auto& terrain_height = refTerrain_->GetHeight(pos.x, pos.z);
		if (pos.y < terrain_height)
		{
			verticalSpeed_ = 0;
			targetTransform_->SetPosition({ Math::Clamp(pos.x - collisionOffset_.x, -400.f, 400.f), terrain_height - height_diff.y, Math::Clamp(pos.z - collisionOffset_.z, -400.f, 400.f) });
			targetCollider_->SetPosition(targetTransform_->GetPosition() + collisionOffset_);
			isAirborne_ = false;

		}
		else
			isAirborne_ = true;
	}
}

void PlayerController::HandleInteraction()
{
	static Input* input = Input::GetInstance();

	static bool greenCrosshair = true;
	bool onPickable = false;

	if (refCamera_->IsActive())
	{
		Interactable* interactable = nullptr;
		Collider* pickable_collider = nullptr;
		RigidBody* pickable_rigidbody = nullptr;


		if (!interactable_)
		{
			const auto& objs = ray_->Raycast();
			//std::cout << objs.size() << "\n";
			for (const auto& i : objs)
			{
				// Maybe choose based on distance
				const auto& obj = i->GetGameObject();
				if (obj->tag == "Interact")
				{
					interactableTag_ = "Interact";
					refButton_ = obj->GetComponent<InteractButton>();
					pickable_collider = i;
					onPickable = true;
					break;
				}
				if (obj->tag == "Pickable" || obj->tag == "Red" || obj->tag == "Green" || obj->tag == "Blue")
				{
					interactableTag_ = obj->tag;
					interactable = obj->GetComponent<Interactable>();
					pickable_collider = i;
					pickable_rigidbody = i->attachedRigidbody;
					onPickable = true;
					break;
				}
			}
		}

		if (input->GetMouseButtonDown(0))
		{
			if (itemHolding_ && onPickable)
			{
				if (!interactable_)
				{
					if (interactableTag_ == "Interact")
					{
						interactable_ = refButton_->Interact();

						if (pickable_collider)
							pickable_collider->SetActive(false);
					}
				}
			}

			if (interactable_ || itemHolding_)
			{
				handle_interaction((interactable_) ? interactable_ : itemHolding_);
			}

			if (onPickable)
			{
				if (interactableTag_ == "Pickable" ||interactableTag_ == "Red"||interactableTag_ == "Green"||interactableTag_ == "Blue"   )
				{
					if (!holdingItem_ && rightArm_)
					{
						RotateArmToHold();
						holdingItem_ = true;
						itemHolding_ = interactable;
						interactableData_.playerItemHolding = itemHolding_;
						if (pickable_collider)
							pickable_collider->SetActive(false);

						if (pickable_rigidbody)
							pickable_rigidbody->SetActive(false);

						Transform* itemTransform = itemHolding_->GetGameObject()->GetTransform();
						itemTransform->SetParent(rightArm_);
						constexpr glm::vec3 size_reset{ 1 / 6.f };
						itemTransform->SetScale(size_reset * itemTransform->GetScale());
						constexpr glm::vec3 righthand_offset = { -0.25f, 0.1f, 0.65f };

						// TODO - Could optimize this because the rotation right now is a constant change.
						const auto& inverseRotation = glm::inverse(rightArm_->GetRotation());
						itemTransform->SetPosition(glm::inverse(rightArm_->GetRotation()) * righthand_offset);
						itemTransform->SetEulerAngles({ 0,0,0 });
						itemTransform->SetRotation(inverseRotation);
					}
				}
				if (interactableTag_ == "Interact")
				{
					if (!interactable_)
					{
						interactable_ = refButton_->Interact();

						if (pickable_collider)
							pickable_collider->SetActive(false);

						handle_interaction(interactable_);
					}
				}
			}
		}
				
		greenCrosshair = onPickable;
		//if (input->GetMouseButtonDown(0))
		//	greenCrosshair = !(greenCrosshair && onPickable);

		constexpr glm::vec4 PINK = glm::vec4(1.0f, 0.f, 1.0f, 1.0f);
		constexpr glm::vec4 GREEN = glm::vec4(.0f, 1.0f, 0.0f, 1.0f);

		if (crosshairMesh_)
		{
			crosshairMesh_->materials.front()->data->_diffuse = greenCrosshair ? GREEN : PINK;
		}
		if (mouseIcon_)
		{
			mouseIcon_->SetActive(greenCrosshair);

			if (greenCrosshair)
			{
				mouseIcon_->GetTransform()->SetPosition(interactable->GetGameObject()->GetTransform()->GetPosition() + glm::vec3(0, 6, 0));
			}
		}
	}


}

void PlayerController::FixedUpdate()
{
	HandleTerrainCollision();

	targetTransform_->SetPosition(targetCollider_->GetPosition() - collisionOffset_);
	//targetTransform_->ForceUpdate();
}

void PlayerController::UpdateGravity()
{
	// Before checking for collision.
	//HandleGravity();
}

void PlayerController::Initialize()
{
	if (targetCollider_)
		targetCollider_->SetPosition(targetTransform_->GetPosition() + collisionOffset_);
}

Ray* PlayerController::GetRay()
{
	return ray_;
}

void PlayerController::BindCamera(Camera* cam)
{
	refCamera_ = cam;
}

void PlayerController::BindCrosshair(std::shared_ptr<GameObject> crosshair)
{
	refCrosshair_ = crosshair;
	const auto& mf = crosshair->GetComponent<MeshFilter>();
	if (mf)
		crosshairMesh_ = mf->GetMesh();
}

void PlayerController::CameraFollowTransform(Transform* other)
{
	followingCameraTransform_ = other;
}

void PlayerController::SetCameraPositionOffset(const glm::vec3& offset)
{
	cameraOffset_ = offset;
}

void PlayerController::BindIcon(GameObject* go)
{
	mouseIcon_ = go;
}

void PlayerController::handle_interaction(Interactable*& i)
{
	if (i->timesUsable == 1)
	{
		++interactableData_.timesUsed;
		interactableData_.useRay = ray_;
		i->Use(interactableData_);

		itemHolding_ = interactableData_.playerItemHolding;
		bool removeItem = false;
		removeItem = itemHolding_ == nullptr;

		if (interactable_ && refButton_)
		{
			refButton_->Reset();
			refButton_ = nullptr;
			interactable_ = nullptr;
		}
		if (removeItem && holdingItem_)
		{
			holdingItem_ = false;
			itemHolding_ = nullptr;
			ResetArm();
		}
		//i = nullptr;

		interactableData_.timesUsed = 0;
	}
	else if (i->timesUsable > interactableData_.timesUsed)
	{
		++interactableData_.timesUsed;
		interactableData_.playerItemHolding = itemHolding_;
		interactableData_.useRay = ray_;
		i->Use(interactableData_);
	}
	else
	{
		++interactableData_.timesUsed;
		interactableData_.useRay = ray_;
		i->Use(interactableData_);

		itemHolding_ = interactableData_.playerItemHolding;
		bool removeItem = false;
		removeItem = itemHolding_ == nullptr;

		if (interactable_ && refButton_)
		{
			refButton_->Reset();
			refButton_ = nullptr;
			interactable_ = nullptr;
		}
		if (removeItem && holdingItem_)
		{
			holdingItem_ = false;
			itemHolding_ = nullptr;
			ResetArm();
		}
		//i = nullptr;

		interactableData_.playerItemHolding = nullptr;
		interactableData_.timesUsed = 0;
	}
}

void PlayerController::RotateArmToHold()
{
	rightArm_->Rotate(glm::angleAxis(glm::radians(90.0f), rightArm_->up));
	rightArm_->Rotate(glm::angleAxis(glm::radians(40.0f), rightArm_->up));
}

void PlayerController::ResetArm()
{
	rightArm_->SetRotation(glm::angleAxis(glm::radians(50.0f), glm::vec3(0, 0, 1)));
}

