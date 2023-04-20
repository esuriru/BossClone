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

constexpr uint8_t NUM_CAMERAS = 2;

class JungleScene : public Scene
{
public:
	JungleScene();
	virtual ~JungleScene() override;

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

	std::array<Camera*, NUM_CAMERAS> cameras_;
	//Camera* _spectatorCamera;
	//Camera* _playerCamera;
	//Camera* _thirdPersonCamera;

	CameraChangeData cameraChangeData_;

	//uint32_t numLights_ = 0;

	ShaderLibrary* shaderLib_ = ShaderLibrary::GetInstance();

	SceneRenderer* renderer_ = SceneRenderer::GetInstance();

	std::vector<LightData*> lights_{};
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

	std::array<Mesh*, NUM_PLAYER_PARTS> playerModel_;
	
	Mesh* fountainMesh_;
	Mesh* ahriMesh_;
	Mesh* rockMesh_;
	Mesh* nanachiMesh_;
	Mesh* houseMesh_;
	Mesh* hutaoMesh_;
	Mesh* rockPileMesh_;
	Mesh* teddyBearMesh_;

	Mesh* lanternMesh_;

	Mesh* treeMesh_;
	Mesh* birchTreeMesh_;
	Mesh* fernMesh_;
	Mesh* flowerPatchMesh_;

	Mesh* flashlightMesh_;
	Mesh* tentMesh_;
	Mesh* sphereMesh_;
	Mesh* sphereMesh2_;
	Mesh* quadMesh_;
	Mesh* quadMesh2_;

	Skybox* skybox_;
#pragma endregion MESHES

#pragma region UI
	Text* fpsCounter_;

	std::shared_ptr<GameObject> dialogueBox_;
	Text* dialogueText_;
#pragma endregion UI

	
};

