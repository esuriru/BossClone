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

constexpr int NUM_CAM = 2;

class MainScene : public Scene
{
public:
	MainScene();
	virtual ~MainScene() override;

	virtual void Init() override;
	virtual void Update(double dt) override;
	virtual void Render() override;
	virtual void Exit() override;


	virtual Light* CreateLight();

	//void QueueInterp();

	static bool bumperCarSceneEnabled;
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

	std::array<Camera*, NUM_CAM> cameras_;
	//Camera* _spectatorCamera;
	//Camera* _playerCamera;
	//Camera* _thirdPersonCamera;

	CameraChangeData cameraChangeData_;

	uint32_t numLights_ = 0;

	ShaderLibrary* shaderLib_;

	SceneRenderer* renderer_ = SceneRenderer::GetInstance();
	std::vector<Material*> mats_;

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
	std::shared_ptr<GameObject> carouselBase_;
	std::shared_ptr<Texture> carouselTexture_;

#pragma region MESHES

	std::array<Mesh*, NUM_PLAYER_PARTS> playerModel_;
	
	enum MainSceneMeshes 
	{
		TENT_MESH,
		CAROUSEL_TOP_MESH,
		CAROUSEL_BASE_MESH,
		STALL_MESH,
		GEM_MESH,
		SPHERE_MESH,
		SPHERE_MESH2,
		QUAD_MESH,
		QUAD_MESH2,
		QUAD_MESH3,
		ARCADE_PORTAL,
		CONFETTI,
		MOUSE_DOWN,
		NUM_MESH,
	};

	std::array<Mesh*, NUM_MESH>  meshList_{};

	Skybox* skybox_;

#pragma endregion MESHES

#pragma region UI
	Text* fpsCounter_;

	std::shared_ptr<GameObject> dialogueBox_;
	Text* dialogueText_;
#pragma endregion UI
};

