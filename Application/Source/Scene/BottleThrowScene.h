#pragma once
#include "Utils/Font.h"
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include "Components/Skybox.h"
#include "Components/Camera.h"
#include "Scene.h"
#include "Renderer/ShaderLibrary.h"
#include "Renderer/SceneRenderer.h"

constexpr uint8_t NUM_CAMERASS = 2;

class BottleThrowScene : public Scene
{
public:
	BottleThrowScene();
	virtual ~BottleThrowScene() override;

	virtual void Init() override;
	virtual void Update(double dt) override;
	virtual void Render() override;
	virtual void Exit() override;


	virtual Light* CreateLight();

	//void QueueInterp();

private:
	void SwitchCamera(Camera* newCamera);

	void CameraInterp();

	bool isChangingCamera_;

	std::shared_ptr<GameObject> obj;
	float sceneTime;
	struct CameraChangeData
	{
		glm::quat oldCameraRotation;
		glm::vec3 oldCameraPosition;

		glm::quat newCameraRotation;
		glm::vec3 newCameraPosition;

		glm::quat currentCameraQuaternion;
		glm::vec3 currentCameraPosition;

		uint32_t frameDuration;
		float inverseFrameDuation;
		uint32_t frameIterator;

		Camera* newCamera;
	};

	std::array<Camera*, NUM_CAMERASS> cameras_;
	//Camera* _spectatorCamera;
	//Camera* _playerCamera;
	//Camera* _thirdPersonCamera;

	CameraChangeData cameraChangeData_;

	uint32_t numLights_ = 0;

	ShaderLibrary* shaderLib_;

	SceneRenderer* renderer_ = SceneRenderer::GetInstance();

	std::vector<LightData*> lights_;
	Font* inconsolataFont_ = nullptr;

	enum PlayerParts
	{
		HEAD,
		LEFT_ARM,
		RIGHT_ARM,
		TORSO,
		LEFT_LEG,
		RIGHT_LEG,
		NUM_PLAYER_PARTS,
	};

	std::shared_ptr<GameObject> playerBase_;

#pragma region MESHES

	std::vector<Material*> mats_;
	std::array<Mesh*, NUM_PLAYER_PARTS> playerModel_;

	Mesh* portal = nullptr;
	
	Mesh* mouseIcon_ = nullptr;
	Mesh* fountainMesh_ = nullptr;
	Mesh* ahriMesh_ = nullptr;
	Mesh* rockMesh_ = nullptr;
	Mesh* stallMesh_ = nullptr;
	Mesh* nanachiMesh_ = nullptr;
	Mesh* houseMesh_ = nullptr;
	Mesh* hutaoMesh_ = nullptr;
	Mesh* rockPileMesh_ = nullptr;
	Mesh* teddyBearMesh_ = nullptr;

	Mesh* lanternMesh_ = nullptr;
	Mesh* bottleMesh_ = nullptr;
	Mesh* stoolMesh_ = nullptr;

	Mesh* treeMesh_ = nullptr;
	Mesh* birchTreeMesh_ = nullptr;
	Mesh* fernMesh_ = nullptr;
	Mesh* flowerPatchMesh_ = nullptr;

	Mesh* flashlightMesh_ = nullptr;
	Mesh* crosshairMesh_ = nullptr;

	Skybox* skybox_ = nullptr;
#pragma endregion MESHES

#pragma region UI
	Text* fpsCounter_;

	std::shared_ptr<GameObject> dialogueBox_;
	Text* dialogueText_;
#pragma endregion UI

	
};

