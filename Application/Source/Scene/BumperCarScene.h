#pragma once
#include "Renderer/Material.h"
#include "Utils/Font.h"
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include "Components/Skybox.h"
#include "Components/Camera.h"
#include "Scene.h"
#include "Renderer/ShaderLibrary.h"
#include "Renderer/SceneRenderer.h"


class BumperCarScene : public Scene
{
public:
	BumperCarScene();
	virtual ~BumperCarScene() override;

	virtual void Init() override;
	virtual void Update(double dt) override;
	virtual void Render() override;
	virtual void Exit() override;


	virtual Light* CreateLight();

	//void QueueInterp();

private:
	const uint8_t NUM_CAM = 2;
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

	std::array<Camera*, 2> cameras_;
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
	std::vector<Material*> mats_;

#pragma region MESHES


	enum BumperMeshList
	{
		SPHERE_MESH,
		B_NUM_MESH
	};
	std::array<Mesh*, B_NUM_MESH> meshList_;
	

	Skybox* skybox_;
#pragma endregion MESHES

#pragma region UI
	Text* fpsCounter_;

	std::shared_ptr<GameObject> dialogueBox_;
	Text* dialogueText_;
#pragma endregion UI

	
};

