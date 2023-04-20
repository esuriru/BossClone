#pragma once
#include "Component.h"
#include "RigidBody.h"
#include "Ray.h"
#include "Renderer/Mesh.h"
#include "Interactable.h"
#include "RigidBody.h"

class Transform;
class Terrain;
class GameObject;
class Camera;
class Interactable;
class InteractButton;
class SphereCollider;

constexpr float gravity_scale = 9.8f;

/// <summary>
/// Will be simple.
/// </summary>
class PlayerController : public RigidBody 
{
public:
	PlayerController(GameObject* go);

	void SetTerrain(std::shared_ptr<Terrain> t);
	virtual void Update(double dt) override;

	virtual void HandleKeyPress();
	void HandleTerrainCollision();
	void HandleGravity();
	void HandleInteraction();

	void FixedUpdate() override;
	void UpdateGravity() override;
	void Initialize();

	Ray* GetRay();

	void BindCamera(Camera* cam);
	void BindCrosshair(std::shared_ptr<GameObject> crosshair);

	void CameraFollowTransform(Transform* other);
	void SetCameraPositionOffset(const glm::vec3& offset);

	void BindIcon(GameObject* go);

private:
	GameObject* mouseIcon_;
	glm::vec3 rotation;
	SphereCollider* sphereCollider_;
	void handle_interaction(Interactable*& i);

	void RotateArmToHold();
	void ResetArm();

	glm::vec3 cameraOffset_{};

	Transform* followingCameraTransform_;

	std::string interactableTag_;
	Interactable* itemHolding_;
	Interactable* interactable_;
	InteractButton* refButton_;

	UseData interactableData_;

	std::shared_ptr<Terrain> refTerrain_;
	Transform* targetTransform_;

	Transform* rightArm_;

	std::shared_ptr<GameObject> refCrosshair_;
	Mesh* crosshairMesh_;

	Camera* refCamera_;
	Ray* ray_;

	glm::vec3 collisionOffset_;

	float deltaTime_;
	float acceleration_;
	float verticalSpeed_;
	float defaultSpeed;
	float runSpeed;
	float currentSpeed;

	bool isAirborne_;
	bool holdingItem_;
};

