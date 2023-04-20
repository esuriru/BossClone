#include "BumperCarScene.h"
#include "Utils/TextureManager.h"
#include "KeyboardController.h"
#include "Utils/Util.h"
#include "Utils/Input.h"
#include "Physics/ColliderManager.h"
#include <Windows.h>
#include "Utils/LoadOBJ.h"
#include "Utils/Input.h"
#include "Components/Terrain.h"


#include "Renderer/MeshBuilder.h"
#include "Components/Text.h"
#include "Components/ModelRenderer.h"

#include <iostream>

#include "Renderer/SceneRenderer.h"

#include "Core/Application.h"

#include "GameObject.h"

// Components
#include "Components/BoxCollider.h"
#include "Components/PanningCameraController.h"
#include "Components/SpotlightController.h"
#include "Components/Ahri.h"
#include "Components/CoveringRock.h"
#include "Components/Talkable.h"
#include "Components/InteractButton.h"
#include "Components/Interactable.h"
#include "Components/ThrowableRock.h"
#include "Components/UIRenderer.h"
#include "Components/PlayerCameraController.h"
#include "Components/CameraController.h"
#include "Components/MeshFilter.h"
#include "Components/MeshRenderer.h"
#include "Components/RigidBody.h"
#include "Components/SphereCollider.h"
#include "Components/PlayerController.h"
#include "Components/BumperCarController.h"
#include "Components/PlayerMovement.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

BumperCarScene::BumperCarScene()
	: shaderLib_()
	, isChangingCamera_(false)
{
	applicationRefreshes_ = 7;
}

void BumperCarScene::Init()
{
	Application::GetInstance()->UpdateLoadingScene("Setting up shaders.");
	shaderLib_ = ShaderLibrary::GetInstance();
	auto envShader = shaderLib_->Get("Environment Map");
	envShader->SetFlags(ShaderFlag::ENVIRONMENT_MAPPED);

	Shader::MultitexturedShader = shaderLib_->Get("Terrain Shader");
	if (Shader::MultitexturedShader)
		Shader::MultitexturedShader->SetFlags(ShaderFlag::MULTI_TEXTURING);

	glm::mat4 projectionMatrix = glm::perspective(45.0f, static_cast<float>(Application::GetInstance()->GetWindowWidth()) / static_cast<float>(Application::GetInstance()->GetWindowHeight()), 0.1f, 1000.0f);

	Application::GetInstance()->UpdateLoadingScene("Setting up cameras.");
	auto spectator_camera = CreateGameObject();
	cameras_[0] = spectator_camera->AddComponent<Camera>();
	spectator_camera->GetTransform()->SetPosition({ 0.f, 300.f, 0.f });
	spectator_camera->GetTransform()->SetRotation(glm::quatLookAt(glm::vec3{0, -1, 0}, glm::vec3{ 1, 0, 0 }));

	cameras_[0]->projection = projectionMatrix;
	sceneCamera_ = cameras_[0];


	Application::GetInstance()->UpdateLoadingScene("Setting up meshes and materials.");
	Material* cubeMaterial = new Material(
		{ 0.3f, 0.3f, 0.3f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 0.5f, 1.0f },
		15.f,
		Shader::DefaultShader
	);

	mats_.push_back(cubeMaterial);

	Material* white = new Material(
		{ 1.0f, 1.f, 1.0f, 1.0f },
		shaderLib_->Get("Illum 0")
	);

	mats_.push_back(white);

	const auto dirlight = CreateLight();
	dirlight->data->direction = { 0.f, -sqrt(2), sqrt(2), 0.f };

	Material* crosshairMaterial = new Material(
		{ 1.0f, 0.f, 1.0f, 1.0f },
		shaderLib_->Get("Illum 0")
	);

	mats_.push_back(crosshairMaterial);

	meshList_[SPHERE_MESH] = MeshBuilder::GenerateSphere("Sphere", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, 32, 1.f);
	auto crosshair = CreateGameObject();
	crosshair->AddComponent<UIRenderer>()->SetMaterial(crosshairMaterial);
	crosshair->GetComponent<UIRenderer>()->renderOrder = 2;
	crosshair->AddComponent<MeshFilter>()->SetMesh(meshList_[SPHERE_MESH]);
	crosshair->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() * 0.5f, Application::GetInstance()->GetWindowHeight() * 0.5f, 0});
	crosshair->GetTransform()->SetScale({ 2, 2, 1 });

	skybox_ = new Skybox({
			"Assets/skybox/posx.tga",
			"Assets/skybox/negx.tga",
			"Assets/skybox/posy.tga",
			"Assets/skybox/negy.tga",
			"Assets/skybox/posz.tga",
			"Assets/skybox/negz.tga"
		});


	std::shared_ptr<GameObject> text = CreateGameObject();
	fpsCounter_ = text->AddComponent<Text>();
	fpsCounter_->SetText("FPS: ");
	text->GetTransform()->SetPosition({ 0, Application::GetInstance()->GetWindowHeight() - 40, 0 });
	text->GetTransform()->SetScale({ 40, 45, 1 });
	text->GetTransform()->SetEulerAngles({ 0, 0, 0 });
	fpsCounter_->font = inconsolataFont_;

	fpsCounter_->onScreen = true;	


	Application::GetInstance()->UpdateLoadingScene("Generating terrain.");
	auto terrain = Terrain::GenerateTerrain(
		"Assets/ring.png",
		"Assets/MainScene/RockTexture1.jpg",
		"Assets/blendMap2.png",
		"Assets/path.png",
		"Assets/path.png",
		"Assets/MainScene/RockTexture2.jpg",
		1.f
	);

	if (terrain)
	{
		objects_.push_back(terrain);

		auto& terrain_mat = objects_.back()->GetComponent<MeshFilter>()->GetMesh()->materials.back();
		terrain_mat->tilingFactor = 40.f;

		auto& terrain_mat_data = terrain_mat->data;
		terrain_mat_data->_specular = { 0.07f, 0.4f, 0.07f, 1.0f };
		terrain_mat_data->_ambient = { 0.15f, 0.15f, 0.15f, 1.0f };
		terrain_mat_data->_shininess = 255.0f;
		terrain_mat_data->_diffuse = {86.f / 255.f, 125.f / 255.f, 70.f / 255.f, 1.0};

		terrain->GetTransform()->SetPosition({ -400, -60, -400 });
		terrain->GetComponent<TerrainCollider>()->Initialize();

		mats_.push_back(terrain->GetComponent<MeshFilter>()->GetMesh()->materials.front());
	}

	Material* envMapMat = new Material();
	envMapMat->materialShader = shaderLib_->Get("Environment Map");

	mats_.push_back(envMapMat);

#pragma region PILLARS
	Application::GetInstance()->UpdateLoadingScene("Generating pillars.");

	std::vector<glm::vec3> pillar_positions
	{
		{-100, terrain->GetHeight(-100, -100), -100},
		{-50, terrain->GetHeight(-50, -100), -100},
		{0, terrain->GetHeight(0, -100), -100},
		{50, terrain->GetHeight(50, -100), -100},
		{100, terrain->GetHeight(100, -100), -100},
		{-100, terrain->GetHeight(-100, 0), 0},
		{-50, terrain->GetHeight(-50, 0), 0},
		{0, terrain->GetHeight(0, 0), 0},
		{50, terrain->GetHeight(50, 0), 0},
		{100, terrain->GetHeight(100, 0), 0},
		{-100, terrain->GetHeight(-100, 100), 100},
		{-50, terrain->GetHeight(-50, 100), 100},
		{0, terrain->GetHeight(0, 100), 100},
		{50, terrain->GetHeight(50, 100), 100},
		{100, terrain->GetHeight(100, 100), 100},
	};

	std::shared_ptr<GameObject> pillarObj = CreateGameObject();
	ModelRenderer* pillarModel = pillarObj->AddComponent<ModelRenderer>();
	pillarModel->LoadModel("Assets/Pillar/pillar.obj");

	pillarObj->GetTransform()->SetPosition(pillar_positions[0]);
	pillarObj->GetTransform()->SetScale({ 10.f, 10.f, 10.f });
	BoxCollider* pillarCol = pillarObj->AddComponent<BoxCollider>();
	pillarCol->Initialize();
	pillarCol->SetSize({ 10.f, 10.f, 10.f });

	for (int i = 1; i < pillar_positions.size(); i++)
	{
		std::shared_ptr<GameObject> pillarObj = CreateGameObject();
		pillarObj->AddComponent<ModelRenderer>()->ReferenceModelRenderer(pillarModel);
	
		pillarObj->GetTransform()->SetPosition(pillar_positions[i]);
		pillarObj->GetTransform()->SetScale({ 10.f, 10.f, 10.f });
		BoxCollider* pillarCol = pillarObj->AddComponent<BoxCollider>();
		pillarCol->Initialize();
		pillarCol->SetSize({ 10.f, 10.f, 10.f });
	}


#pragma endregion PILLARS

#pragma region BALL
	Application::GetInstance()->UpdateLoadingScene("Generating balls.");

	std::vector<glm::vec3> ballPositions
	{
		{50.f, 5.f, 50.f},
		{100.f, 5.f, 10.f},
		{30.f, 5.f, 70.f},
		{-150.f, 5.f, -100.f},
		{150.f, 5.f, 90.f},
	};

	std::shared_ptr<GameObject> ballObj = CreateGameObject();
	ModelRenderer* ballModel = ballObj->AddComponent<ModelRenderer>();
	ballModel->LoadModel("Assets/Ball/ball.obj");

	ballObj->GetTransform()->SetPosition({50.f, 5.f, -90.f});
	ballObj->GetTransform()->SetScale({45.f, 45.f, 45.f});
	ballObj->AddComponent<RigidBody>();
	ballObj->GetComponent<RigidBody>()->SetMass(1.f);
	ballObj->GetComponent<RigidBody>()->SetRestitution(0.8f);
	SphereCollider* ballCol = ballObj->AddComponent<SphereCollider>();
	ballCol->Initialize();
	ballCol->SetRadius(9.f);

	for (int i = 1; i < ballPositions.size(); i++)
	{
		std::shared_ptr<GameObject> ballObj1 = CreateGameObject();
		ballObj1->AddComponent<ModelRenderer>()->ReferenceModelRenderer(ballModel);
		ballObj1->AddComponent<RigidBody>();
		ballObj1->GetComponent<RigidBody>()->SetMass(1.f);
		ballObj1->GetComponent<RigidBody>()->SetRestitution(0.8f);

		ballObj1->GetTransform()->SetPosition(ballPositions[i]);
		ballObj1->GetTransform()->SetScale({ 45.f, 45.f, 45.f });
		SphereCollider* ballCol1 = ballObj1->AddComponent<SphereCollider>();
		ballCol1->Initialize();
		ballCol1->SetRadius(9.f);
	}
	

#pragma endregion BALL
#pragma region BUMPER_CAR_MODEL
	Application::GetInstance()->UpdateLoadingScene("Generating the bumper car.");

	std::shared_ptr<GameObject> bumperCar = CreateGameObject();
	bumperCar->GetTransform()->SetScale({ 1.f, 1.f, 1.f });
	bumperCar->GetTransform()->SetPosition({ 10.f, 0.f, 10.f});
	bumperCar->AddComponent<BumperCarController>();
	bumperCar->AddComponent<RigidBody>();
	bumperCar->GetComponent<RigidBody>()->SetMass(5.f);
	bumperCar->GetComponent<RigidBody>()->SetRestitution(0.3f);

	std::shared_ptr<GameObject> bumperCarModelObj = CreateGameObject();
	bumperCarModelObj->AddComponent<ModelRenderer>()->LoadModel("Assets/BumperCar/Car.obj");
	bumperCarModelObj->GetTransform()->SetEulerAngles({ 0 ,90 , 0 });
	bumperCarModelObj->GetTransform()->SetScale({ 8.f, 8.f, 8.f });
	bumperCarModelObj->GetTransform()->SetParent(bumperCar->GetTransform());

	auto carCol = bumperCar->AddComponent<SphereCollider>();
	carCol->SetPosition({ bumperCar->GetTransform()->GetPosition() });
	carCol->SetRadius(12.f);
	carCol->SetOffset({ 0.f, 12.f, 0.f });
	carCol->Initialize();
	
#pragma endregion BUMPER_CAR_MODEL

	//Update(0.01);
	sceneCamera_ = cameras_[0];
}

void BumperCarScene::Update(double dt)
{
	constexpr double fps_update_time = 1.0;
	static double timer = 0.0;
	timer += dt;
	if (timer > fps_update_time)
	{
		fpsCounter_->SetText("FPS: " + std::to_string(static_cast<int>(1.0 / dt)));
		timer -= fps_update_time;
	}

	static double timePassed = 0;
	timePassed += dt;

	if (timePassed >= FIXED_UPDATE_FRAME_TIME)
	{
		for (const auto& i : objects_)
		{
			if (i->ActiveSelf())
			{
				i->BeforeFixedUpdate(dt);
			}
		}

		static ColliderManager* collider_manager = ColliderManager::GetInstance();
		collider_manager->Update(dt);

		for (const auto& i : objects_)
		{
			if (i->ActiveSelf())
			{
				i->FixedUpdate(dt);
			}
		}

		timePassed -= FIXED_UPDATE_FRAME_TIME;
	}

	for (const auto& i : objects_)
	{
		if (i->ActiveSelf())
		{
			i->Update(dt);
		}
		if (isChangingScene_)
			return;
	}

	CameraInterp();

	static Input* input = Input::GetInstance();
	input->Update();

}

void BumperCarScene::Render()
{
	if (Input::GetInstance()->IsKeyDown('B'))
	{
		Application::GetInstance()->LoadScene("MainScene");
		return;
	}
	renderer_->Clear();
	renderer_->SetClearColour({ 189.f / 255.f, 212.f / 255.f, 230.f / 255.f, 0.0f });


	if (sceneCamera_)
	{
		if (!isChangingCamera_)
			renderer_->BeginScene(sceneCamera_, shaderLib_, skybox_, lights_, lights_.size());
		else
			renderer_->BeginScene(
				glm::inverse(
					glm::translate(glm::mat4(1.0f), cameraChangeData_.currentCameraPosition) * glm::mat4_cast(cameraChangeData_.currentCameraQuaternion)
				),
				sceneCamera_->projection,
				cameraChangeData_.currentCameraPosition,
				shaderLib_, skybox_, lights_,lights_.size()
			);
	}


	for (const auto& i : objects_)
	{
		if (i->ActiveSelf())
			i->Render();
	}

	renderer_->EndScene();
}

void BumperCarScene::Exit()
{
	TextureManager::GetInstance()->ClearTextures();
	//for (auto& i : objects_)
	//	if (i)
	//	{
	//		delete i;
	//		i = nullptr;
	//	}

	if (skybox_)
	{
		delete skybox_;
		skybox_ = nullptr;
	}

	delete inconsolataFont_;
	inconsolataFont_ = nullptr;
	CC_TRACE("Deleting mesh list.");
	for (auto& i : meshList_)
	{ 
		delete i;
		i = nullptr;
	}

	lights_.clear();
	objects_.clear();
	
	for (auto& i : mats_)
	{
		delete i;
		i = nullptr;
	}
	mats_.clear();
}



Light* BumperCarScene::CreateLight()
{
	std::shared_ptr<GameObject> temp = CreateGameObject();
	auto temp_light = temp->AddComponent<Light>(); 	
	lights_.emplace_back(temp_light->data);
	//lights_[numLights_] = temp_light->data;
	//++numLights_;
	return temp_light;
}

void BumperCarScene::CameraInterp()
{
	if (!isChangingCamera_)
		return;

	const float& it = cameraChangeData_.inverseFrameDuation * cameraChangeData_.frameIterator;

	cameraChangeData_.currentCameraPosition =
		Util::Lerp(cameraChangeData_.oldCameraPosition, cameraChangeData_.newCameraPosition, it);

	cameraChangeData_.currentCameraQuaternion =
		glm::slerp(cameraChangeData_.oldCameraRotation, cameraChangeData_.newCameraRotation, it);

	if (cameraChangeData_.frameIterator >= cameraChangeData_.frameDuration)
	{
		sceneCamera_->SetActive(false);
		sceneCamera_ = cameraChangeData_.newCamera;
		sceneCamera_->SetActive(true);
		isChangingCamera_ = false;
		return;
	}

	++cameraChangeData_.frameIterator;
}

void BumperCarScene::SwitchCamera(Camera* newCamera)
{
	cameraChangeData_.oldCameraPosition = sceneCamera_->GetGameObject()->GetTransform()->GetPosition();
	cameraChangeData_.oldCameraRotation = sceneCamera_->GetGameObject()->GetTransform()->GetRotation();
	cameraChangeData_.currentCameraPosition = cameraChangeData_.oldCameraPosition;
	cameraChangeData_.currentCameraQuaternion = cameraChangeData_.oldCameraRotation;
	cameraChangeData_.newCameraPosition = newCamera->GetGameObject()->GetTransform()->GetPosition();
	std::cout << glm::to_string(cameraChangeData_.newCameraPosition) << "\n";
	cameraChangeData_.newCameraRotation = newCamera->GetGameObject()->GetTransform()->GetRotation();

	constexpr uint32_t CAMERA_CHANGE_DURATION = 288;
	cameraChangeData_.frameIterator = 0;
	cameraChangeData_.frameDuration = CAMERA_CHANGE_DURATION;
	cameraChangeData_.inverseFrameDuation = 1.f / cameraChangeData_.frameDuration;

	cameraChangeData_.newCamera = newCamera;

	isChangingCamera_ = true;
}

BumperCarScene::~BumperCarScene()
{
}
