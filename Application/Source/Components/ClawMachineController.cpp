#include "ClawMachineController.h"
#include "Scene/GameObject.h"
#include "Transform.h"
#include "Utils/Input.h"
#include <glm/gtx/string_cast.hpp>
#include "Core/Log.h"
#include "GLFW/glfw3.h"

ClawMachineController::ClawMachineController(GameObject* go)
	: Component(go)
	, targetTransform_(go->GetTransform())
	, targetChildTransform1_(go->GetTransform()->GetChild(0))
	, targetChildTransform2_(go->GetTransform()->GetChild(1))
	, targetChildTransform3_(go->GetTransform()->GetChild(2))
	, down(false)
	, up(false)
{
}

void ClawMachineController::Update(double dt)
{
	
	deltaTime_ = static_cast<float>(dt);

	HandleKeyPress();
}

void ClawMachineController::HandleKeyPress()
{

	// Loop for interaction
	// 1. WASD to move the claw
	// 2. On space, claw will extend and "fingers" will unfold
	// 3. On space release, claw will retract and bring the object, with curled fingers, to the origin point

	// First we initialise the variables
	// Direction vectors for movement
	// Facing front, which is -1 on z-axis considering front orientation of the machine
	constexpr glm::vec3 front{ glm::vec3(0.f, 0.f, -1.f) };
	// Facing right, which is 1 on x-axis considering front orientation of the machine
	constexpr glm::vec3 right{ glm::vec3(1.f,0.f,0.f) };
	// Facing down, which is -1 on y-axis
	constexpr glm::vec3 down{ glm::vec3(0.f,-1.f,0.f) };

	// Boolean flag for indicating if the claw is resetting to original pos and is not yet original pos
	static bool clawResetting{ false };

	// Boolean flag for determining if the machine is waiting for the spacebar release
	static bool spaceDown{ false };

	// Boolean flag for determining if the claw should be at original pos (that is, initializing the claw position on start)
	static bool initDone{ false };

	// rotation speed for the claw "extending" motion
	constexpr float rotateSpeed = 45.f;

	// The position of the claw (origin)
	constexpr glm::vec3 originPos{ glm::vec3(-1.f, 0.f, 1.f) };

	// Velocity of the claw as it is moving back
	constexpr float clawResetVel{ 5.f };
	// Velocity of the claw as it is rising
	constexpr float risingVel{ 2.f };
	// Velocity of the claw as it moves
	constexpr float clawMoveVel{ 5.f };

	// Rotations for the front and back hands of the claw 
	constexpr glm::vec3 frontEulerAngles{ glm::vec3(-1.f,0.f,0.f) };
	constexpr glm::vec3 backEulerAngles{ glm::vec3(2.f,0.f,0.f) };

	// Limit to rotation for front and back hands of the claw
	constexpr glm::vec3 frontAngleLimit{ glm::vec3(1.5f,0.f,0.f) };
	constexpr glm::vec3 backAngleLimit{ glm::vec3(-0.5f,0.f,0.f) };

	// Instance of the Input class
	static Input* input = Input::GetInstance();

	// Initialize the claw position
	if (!initDone)
	{
		targetTransform_->Translate(originPos);

		targetChildTransform1_->SetEulerAngles(frontEulerAngles);
		targetChildTransform2_->SetEulerAngles(frontEulerAngles);
		targetChildTransform3_->SetEulerAngles(backEulerAngles);

		initDone = true;
	}

	if (!clawResetting)
	{
		if (!spaceDown)
		{
			if (input->IsKeyDown('W') && targetTransform_->GetPosition().z > -1.25f)
			{
				targetTransform_->Translate(front * clawMoveVel * deltaTime_);
			}
			if (input->IsKeyDown('S') && targetTransform_->GetPosition().z < originPos.z)
			{
				targetTransform_->Translate(-front * clawMoveVel * deltaTime_);
			}
			if (input->IsKeyDown('A') && targetTransform_->GetPosition().x > originPos.x)
			{
				targetTransform_->Translate(-right * clawMoveVel * deltaTime_);
			}
			if (input->IsKeyDown('D') && targetTransform_->GetPosition().x < 1.325f)
			{
				targetTransform_->Translate(right * clawMoveVel * deltaTime_);
			}
		}

		if (input->IsKeyDown(GLFW_KEY_SPACE))
		{
			spaceDown = true;
		}

		if (input->IsKeyReleased(GLFW_KEY_SPACE))
		{
			spaceDown = false;
			clawResetting = true;
		}
	}

	// Handle translations according to spaceDown and clawResetting
	if (spaceDown)
	{
		// Move downwards
		// Set limit to downward translate
		if (targetTransform_->GetPosition().y > -1.f)
		{
			targetTransform_->Translate(down * risingVel * deltaTime_);
		}

		// Rotate (Expand the claw)
		glm::vec3 frontRot = targetChildTransform1_->GetEulerAngles();
		glm::vec3 backRot = targetChildTransform3_->GetEulerAngles();


		// Essentially we just rotate along x to make the claw detach a little

		frontRot.x += rotateSpeed * deltaTime_;
		if (frontRot.x > frontAngleLimit.x)
		{
			frontRot.x = frontAngleLimit.x;
		}
		backRot.x -= rotateSpeed * deltaTime_;
		if (backRot.x < backAngleLimit.x)
		{
			backRot.x = backAngleLimit.x;
		}

		targetChildTransform1_->SetEulerAngles(frontRot);
		targetChildTransform2_->SetEulerAngles(frontRot);
		targetChildTransform3_->SetEulerAngles(backRot);
	}
	if (clawResetting)
	{
		// Bring the claw back up
		if (targetTransform_->GetPosition().y < 0.f)
		{
			targetTransform_->Translate(-down * risingVel * deltaTime_);
			if (targetTransform_->GetPosition().y > 0.f)
			{
				targetTransform_->SetPosition(glm::vec3(targetTransform_->GetPosition().x, 0.f, targetTransform_->GetPosition().z));
			}

			// Release (uncoil) the claw
			glm::vec3 frontRot = targetChildTransform1_->GetEulerAngles();
			frontRot.x -= rotateSpeed * deltaTime_;
			if (frontRot.x < frontEulerAngles.x)
			{
				frontRot.x = frontEulerAngles.x;
			}

			glm::vec3 backRot = targetChildTransform3_->GetEulerAngles();
			backRot.x += rotateSpeed * deltaTime_;
			if (backRot.x > backEulerAngles.x)
			{
				backRot.x = backEulerAngles.x;
			}

			targetChildTransform1_->SetEulerAngles(frontRot);
			targetChildTransform2_->SetEulerAngles(frontRot);
			targetChildTransform3_->SetEulerAngles(backRot);
		}
		else
		{
			// Bring the claw position back to origin pos
			if (targetTransform_->GetPosition().x > originPos.x)
			{
				targetTransform_->Translate(-right * clawResetVel * deltaTime_);
			}
			if (targetTransform_->GetPosition().z < originPos.z)
			{
				targetTransform_->Translate(-front * clawResetVel * deltaTime_);
			}

			// Handle over-subtraction or addition for crossing the limit
			float x, y, z;
			x = targetTransform_->GetPosition().x;
			y = targetTransform_->GetPosition().y;
			z = targetTransform_->GetPosition().z;

			if (x < originPos.x)
			{
				targetTransform_->SetPosition(glm::vec3(originPos.x, y, z));
				x = originPos.x;
			}
			if (z > originPos.z)
			{
				targetTransform_->SetPosition(glm::vec3(x, y, originPos.z));
			}
		}
	}

	// Check if at origin
	if (clawResetting && targetTransform_->GetPosition() == originPos)
	{
		clawResetting = false;
	}
}