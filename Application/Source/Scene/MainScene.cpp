#include "MainScene.h"
#include "Utils/TextureManager.h"
#include "Scene/GameObject.h"
#include "KeyboardController.h"
#include "Utils/Util.h"
#include "Utils/Input.h"
#include "Physics/ColliderManager.h"
#include <Windows.h>
#include "Utils/LoadOBJ.h"
#include "Components/Terrain.h"


#include "Renderer/MeshBuilder.h"
#include "Components/Text.h"

#include <iostream>

#include "Renderer/SceneRenderer.h"

#include "Core/Application.h"

#include "GameObject.h"

// Components
#include "Components/Confetti.h"
#include "Components/Portal.h"
#include "Components/FinishPoint.h"
#include "Components/CarouselMovement.h"
#include "Components/DebugMovement.h"
#include "Components/ModelRenderer.h"
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
#include "Components/PlayerMovement.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

bool MainScene::bumperCarSceneEnabled = false;

MainScene::MainScene()
	: shaderLib_()
	, isChangingCamera_(false)
{
	// Hardcoded
	applicationRefreshes_ = 20;
}

void MainScene::Init()
{
#pragma region SHADERS
	Application::GetInstance()->UpdateLoadingScene("Setting up shaders.");
	shaderLib_ = ShaderLibrary::GetInstance();
	auto envShader = shaderLib_->Get("Environment Map");
	envShader->SetFlags(ShaderFlag::ENVIRONMENT_MAPPED);

	Shader::MultitexturedShader = shaderLib_->Get("Terrain Shader");
	if (Shader::MultitexturedShader)
		Shader::MultitexturedShader->SetFlags(ShaderFlag::MULTI_TEXTURING);

#pragma endregion

#pragma region CAMERA
	Application::GetInstance()->UpdateLoadingScene("Setting up cameras.");
	std::shared_ptr<GameObject> mainCamera = CreateGameObject();

	glm::mat4 projectionMatrix = glm::perspective(45.0f, static_cast<float>(Application::GetInstance()->GetWindowWidth()) / static_cast<float>(Application::GetInstance()->GetWindowHeight()), 0.1f, 1000.0f);
	sceneCamera_ = mainCamera->AddComponent<Camera>();
	auto camera_controller = mainCamera->AddComponent<PlayerCameraController>();
	//mainCamera->AddComponent<CameraController>();
	sceneCamera_->projection = projectionMatrix;
	cameras_[0] = sceneCamera_;

	auto spectator_camera = CreateGameObject();
	cameras_[1] = spectator_camera->AddComponent<Camera>();
	spectator_camera->AddComponent<CameraController>();
	spectator_camera->GetTransform()->SetPosition({ 0, 200, 0 });

	//auto tpc = CreateGameObject();
	//_cameras[2] = tpc->AddComponent<Camera>();
	//auto pcc = tpc->AddComponent<PanningCameraController>();

	cameras_[1]->SetActive(false);
	cameras_[1]->projection = projectionMatrix;
#pragma endregion


#pragma region PLAYER_MODEL
	Application::GetInstance()->UpdateLoadingScene("Setting up player model.");
	playerModel_[HEAD] = MeshBuilder::GenerateOBJMTL("Head", "Assets/player/head.obj", "Assets/player/head.mtl");
	playerModel_[LEFT_ARM] = MeshBuilder::GenerateOBJMTL("Left arm", "Assets/player/leftarm.obj", "Assets/player/leftarm.mtl");
	playerModel_[RIGHT_ARM] = MeshBuilder::GenerateOBJMTL("Right arm", "Assets/player/rightarm.obj", "Assets/player/rightarm.mtl");
	playerModel_[LEFT_LEG] = MeshBuilder::GenerateOBJMTL("Left leg", "Assets/player/leftleg.obj", "Assets/player/leftleg.mtl");
	playerModel_[RIGHT_LEG] = MeshBuilder::GenerateOBJMTL("Right leg", "Assets/player/rightleg.obj", "Assets/player/rightleg.mtl");
	playerModel_[TORSO] = MeshBuilder::GenerateOBJMTL("Torso", "Assets/player/torso.obj", "Assets/player/torso.mtl");

	mats_.insert(mats_.end(), playerModel_[HEAD]->materials.begin(), playerModel_[HEAD]->materials.end());
	mats_.insert(mats_.end(), playerModel_[LEFT_ARM]->materials.begin(), playerModel_[LEFT_ARM]->materials.end());
	mats_.insert(mats_.end(), playerModel_[RIGHT_ARM]->materials.begin(), playerModel_[RIGHT_ARM]->materials.end());
	mats_.insert(mats_.end(), playerModel_[LEFT_LEG]->materials.begin(), playerModel_[LEFT_LEG]->materials.end());
	mats_.insert(mats_.end(), playerModel_[RIGHT_LEG]->materials.begin(), playerModel_[RIGHT_LEG]->materials.end());
	mats_.insert(mats_.end(), playerModel_[TORSO]->materials.begin(), playerModel_[TORSO]->materials.end());



	for (int i = 0; i < NUM_PLAYER_PARTS; ++i)
	{
		const auto& j = playerModel_[i];
		if (!j) continue;
		if (j->materials.empty()) continue;
		j->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/player/texture.png");
	}
#pragma endregion

#pragma region MISC
	
	Application::GetInstance()->UpdateLoadingScene("Setting up miscellaneous things.");
	inconsolataFont_ = Font::LoadFont("Assets/fonts/inconsolata.tga", "Assets/fonts/inconsolata.csv");
	inconsolataFont_->fontMultiplier = 0.014f;
	//inconsolataFont_->fontMultiplier = 0.028f;

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


	Material* textureCube = new Material(
		{ 0.6f, 0.6f, 0.6f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 0.5f, 1.0f },
		95.f,
		Shader::DefaultShader
	);

	mats_.push_back(textureCube);

	Material* crosshairMaterial = new Material(
		{ 1.0f, 0.f, 1.0f, 1.0f },
		shaderLib_->Get("Illum 0")
	);

	mats_.push_back(crosshairMaterial);

	meshList_[CONFETTI] = MeshBuilder::GenerateQuad("Confetti", glm::vec4(1.0f));
	mats_.push_back(new Material());
	mats_.back()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Confetti.png");
	mats_.back()->materialShader = ShaderLibrary::GetShaderFromIllumModel(0);
	meshList_[CONFETTI]->materials.emplace_back(mats_.back());

	auto confetti = CreateGameObject();
	confetti->AddComponent<MeshFilter>()->SetMesh(meshList_[CONFETTI]);
	confetti->AddComponent<UIRenderer>()->billboarded = false;
	confetti->AddComponent<Confetti>();
	confetti->SetActive(false);

	meshList_[QUAD_MESH3] = MeshBuilder::GenerateQuad("Crosshair", glm::vec4(1.0f));

	auto crosshair = CreateGameObject();
	crosshair->AddComponent<UIRenderer>()->SetMaterial(crosshairMaterial);
	crosshair->GetComponent<UIRenderer>()->renderOrder = 2;
	crosshair->AddComponent<MeshFilter>()->SetMesh(meshList_[QUAD_MESH3]);
	crosshair->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() * 0.5f, Application::GetInstance()->GetWindowHeight() * 0.5f, 0});
	crosshair->GetTransform()->SetScale({ 2, 2, 1 });

	textureCube->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/question_mark.png");

	std::shared_ptr<GameObject> question_mark = CreateGameObject();
	auto tc_renderer = question_mark->AddComponent<UIRenderer>();
	tc_renderer->SetMaterial(textureCube);
	tc_renderer->billboarded = true;
	
	meshList_[QUAD_MESH] = MeshBuilder::GenerateQuad("Question Mark", glm::vec4(1.0f), 1.0f);
	question_mark->AddComponent<MeshFilter>()->SetMesh(meshList_[QUAD_MESH]);
	question_mark->GetTransform()->SetPosition({ 65.0f, -2.f, 100.f });
	question_mark->GetTransform()->SetEulerAngles({ -90.f, 0.f, 0.f });
	question_mark->GetTransform()->SetScale({ 2.5f, 2.5f, 1.f });
	auto qc = question_mark->AddComponent<SphereCollider>();
	qc->Initialize();
	auto ib = question_mark->AddComponent<InteractButton>();
	question_mark->tag = "Interact";

	Material* dialogueBoxMaterial = new Material(
		{ 1.0f, 0.f, 1.0f, 1.0f },
		shaderLib_->Get("Illum 0")
	);
	mats_.push_back(dialogueBoxMaterial);

	meshList_[QUAD_MESH2] = MeshBuilder::GenerateQuad("Question Mark", glm::vec4(1.0f), 1.0f);
	//dialogueBoxMaterial->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/dialoguebox.png");
	//dialogueBox_ = CreateGameObject();
	//dialogueBox_->tag = "Dialogue Box";
	//dialogueBox_->AddComponent<UIRenderer>()->renderOrder = 2;
	//dialogueBox_->GetComponent<UIRenderer>()->SetMaterial(dialogueBoxMaterial);
	//dialogueBox_->AddComponent<MeshFilter>()->SetMesh(meshList_[QUAD_MESH2]);
	//dialogueBox_->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() *0.5f, 120, 0 });
	//dialogueBox_->GetTransform()->SetScale({ Application::GetInstance()->GetWindowWidth() * 0.8f, Application::GetInstance()->GetWindowHeight() * 0.25f, 0});

	//auto dialogueTextObject = CreateGameObject();
	//dialogueText_ = dialogueTextObject->AddComponent<Text>();
	//dialogueText_->renderOrder = 2;
	//dialogueText_->BindDialogueBox(dialogueBox_);
	//dialogueText_->EnableDialogueBox(false);
	//dialogueText_->font = inconsolataFont_;
	//dialogueText_->onScreen = true;
	//dialogueTextObject->GetTransform()->SetScale({ 50, 60, 1 });
	//dialogueTextObject->GetTransform()->SetPosition({ 185, 120, 0 });

	skybox_ = new Skybox({
			"Assets/MainScene/GalaxyPanorama/px.png",
			"Assets/MainScene/GalaxyPanorama/nx.png",
			"Assets/MainScene/GalaxyPanorama/py.png",
			"Assets/MainScene/GalaxyPanorama/ny.png",
			"Assets/MainScene/GalaxyPanorama/pz.png",
			"Assets/MainScene/GalaxyPanorama/nz.png"
		});


	std::shared_ptr<GameObject> text = CreateGameObject();
	fpsCounter_ = text->AddComponent<Text>();
	fpsCounter_->SetText("FPS: ");
	text->GetTransform()->SetPosition({ 0, Application::GetInstance()->GetWindowHeight() - 40, 0 });
	text->GetTransform()->SetScale({ 40, 45, 1 });
	text->GetTransform()->SetEulerAngles({ 0, 0, 0 });
	fpsCounter_->font = inconsolataFont_;

	fpsCounter_->onScreen = true;	

#pragma endregion


#pragma region TERRAIN
	Application::GetInstance()->UpdateLoadingScene("Generating terrain.");
	auto terrain = Terrain::GenerateTerrain(
		"Assets/CaveHeight256.png",
		"Assets/MainScene/RockTexture1.jpg",
		"Assets/MainScene/caveterrainblendmap1.png",
		"Assets/path.png",
		"Assets/path.png",
		"Assets/MainScene/RockTexture2.jpg",
		5.0f
	);
	Application::GetInstance()->UpdateLoadingScene("Setting up terrain.");

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
#pragma endregion


	Material* envMapMat = new Material();
	envMapMat->materialShader = shaderLib_->Get("Environment Map");
	mats_.push_back(envMapMat);

	
	meshList_[SPHERE_MESH2] = MeshBuilder::GenerateSphere("Reward", glm::vec4(1.0f), 16, 32, 1.0f);
	for (int i = 0; i < 3; ++i)
	{
		auto reward = CreateGameObject();
		reward->AddComponent<MeshRenderer>()->SetMaterial(envMapMat);
		reward->AddComponent<MeshFilter>()->SetMesh(meshList_[SPHERE_MESH2]);
		reward->GetTransform()->SetPosition(glm::vec3(50.f, terrain->GetHeight(50.f, 50.f) + i * 25.f, 50.f));
		reward->GetTransform()->SetScale(glm::vec3(3.f));
		reward->AddComponent<SphereCollider>();
		auto rewardrb = reward->AddComponent<RigidBody>();
		rewardrb->SetMass(1.f);
		rewardrb->SetRestitution(0.9f);
	}

	meshList_[MOUSE_DOWN] = MeshBuilder::GenerateQuad("Mouse Icon", glm::vec4(1.0f));
	mats_.push_back(new Material());
	mats_.back()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/mouse.png");
	mats_.back()->materialShader = ShaderLibrary::GetShaderFromIllumModel(0);
	meshList_[MOUSE_DOWN]->materials.push_back(mats_.back());

	auto mouse_down = CreateGameObject();
	mouse_down->AddComponent<MeshFilter>()->SetMesh(meshList_[MOUSE_DOWN]);
	mouse_down->AddComponent<UIRenderer>()->billboarded = true;
	mouse_down->GetComponent<UIRenderer>()->renderOrder = 3;
	mouse_down->GetTransform()->SetScale({ 4, 4, 1 });
	mouse_down->SetActive(false);

	playerBase_ = CreateGameObject();
	playerBase_->GetTransform()->SetScale({ 1, 1, 1 });
	playerBase_->GetTransform()->SetPosition({ -188, terrain->GetHeight(-188, 209), 209});
	auto pc = playerBase_->AddComponent<PlayerController>();
	pc->SetTerrain(terrain);
	pc->BindCamera(cameras_[0]);
	pc->BindCrosshair(crosshair);
	pc->BindIcon(mouse_down.get());
	pc->Initialize();


	//pcc->SetToLookAt(_playerBase->GetTransform());

#pragma region PLAYER_MODEL
	Application::GetInstance()->UpdateLoadingScene("Linking player model.");

	auto playerTorso = CreateGameObject();
	playerTorso->GetTransform()->SetParent(playerBase_->GetTransform());
	playerTorso->AddComponent<MeshFilter>()->SetMesh(playerModel_[TORSO]);
	playerTorso->AddComponent<MeshRenderer>();
	playerTorso->GetTransform()->SetScale({ 6, 6, 6 });
	playerTorso->GetTransform()->SetPosition({ 0, -1, 0 });
	playerTorso->GetTransform()->SetEulerAngles({ 0, 180, 0 });

	auto playerHead = CreateGameObject();
	playerHead->GetTransform()->SetParent(playerTorso->GetTransform());
	playerHead->AddComponent<MeshFilter>()->SetMesh(playerModel_[HEAD]);
	playerHead->AddComponent<MeshRenderer>();
	playerHead->GetTransform()->SetPosition({ 0, -0.005f, 0 });

	auto playerLeftArmPivot = CreateGameObject();
	playerLeftArmPivot->GetTransform()->SetParent(playerTorso->GetTransform());
	//playerLeftArmPivot->AddComponent<MeshFilter>()->SetMesh(MeshBuilder::GenerateSphere("Test", glm::vec4(), 32, 16, 0.1f));
	//playerLeftArmPivot->AddComponent<MeshRenderer>()->SetMaterial(white);
	//playerLeftArmPivot->GetTransform()->SetScale({ 0.1f, 0.1f, 0.1f });
	playerLeftArmPivot->GetTransform()->SetPosition({ 0.20f, 1.425f, 0 });
	playerLeftArmPivot->GetTransform()->SetEulerAngles({ 0, 0, -50 });
	//playerLeftArmPivot->GetTransform()->SetPosition({ 0.35f, 1.5f, 0 });

	auto playerLeftArm = CreateGameObject();
	playerLeftArm->GetTransform()->SetParent(playerLeftArmPivot->GetTransform());
	playerLeftArm->AddComponent<MeshFilter>()->SetMesh(playerModel_[LEFT_ARM]);
	playerLeftArm->AddComponent<MeshRenderer>();
	//playerLeftArmPivot->GetTransform()->SetScale({ 10.f, 10.f, 10.f });
	playerLeftArm->GetTransform()->SetPosition({ -0.20f, -1.425f, 0 });

	auto playerRightArmPivot = CreateGameObject();
	playerRightArmPivot->GetTransform()->SetParent(playerTorso->GetTransform());
	playerRightArmPivot->GetTransform()->SetPosition({ -0.20f, 1.425f, 0 });
	//playerRightArmPivot->GetTransform()->SetEulerAngles({ 90, 0, 50 });
	playerRightArmPivot->GetTransform()->SetRotation(glm::angleAxis(glm::radians(50.0f), glm::vec3(0, 0, 1)));
	//playerRightArmPivot->GetTransform()->Rotate(glm::angleAxis(glm::radians(90.0f), playerRightArmPivot->GetTransform()->up));
	//playerRightArmPivot->GetTransform()->Rotate(glm::angleAxis(glm::radians(40.0f), playerRightArmPivot->GetTransform()->up));

	//playerRightArmPivot->GetTransform()->SetRotation();

	auto playerRightArm = CreateGameObject();
	playerRightArm->GetTransform()->SetParent(playerRightArmPivot->GetTransform());
	playerRightArm->AddComponent<MeshFilter>()->SetMesh(playerModel_[RIGHT_ARM]);
	playerRightArm->AddComponent<MeshRenderer>();
	playerRightArm->GetTransform()->SetPosition({ 0.20f, -1.425f, 0 });

	auto playerLeftLeg = CreateGameObject();
	playerLeftLeg->GetTransform()->SetParent(playerTorso->GetTransform());
	playerLeftLeg->AddComponent<MeshFilter>()->SetMesh(playerModel_[LEFT_LEG]);
	playerLeftLeg->AddComponent<MeshRenderer>();

	auto playerRightLeg = CreateGameObject();
	playerRightLeg->GetTransform()->SetParent(playerTorso->GetTransform());
	playerRightLeg->AddComponent<MeshFilter>()->SetMesh(playerModel_[RIGHT_LEG]);
	playerRightLeg->AddComponent<MeshRenderer>();

	//playerTorso->GetTransform()->SetPosition({ 0, 0, 0 });
#pragma endregion 

	camera_controller->Follow(playerBase_->GetTransform());
	//pc->CameraFollowTransform(playerHead->GetTransform());
	pc->SetCameraPositionOffset({ 0, 9.5f, 0 });
	//tentMesh_ = MeshBuilder::GenerateOBJMTL("Tent", "Assets/Carousel/Obj/CarouselTop2.obj", "Assets/Carousel/Obj/CarouselTop2.mtl");
	//tentMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Carousel/Img/CarouselDiffuse.png");
	//for (int i = 1 ; i < 39; ++i)
	//	tentMesh_->materials[i]->textureMaps[Material::DIFFUSE] = tentMesh_->materials[0]->textureMaps[Material::DIFFUSE];
	//auto ahri = CreateGameObject();
	//ahri->AddComponent<ModelRenderer>()->LoadModel("Assets/Ahri/source/Ahri/Ahri.obj");
	////tent->AddComponent<MeshFilter>()->SetMesh(tentMesh_);
	//ahri->GetTransform()->SetPosition(glm::vec3(1.f, -10.f, 1.f));
	//ahri->GetTransform()->SetScale(glm::vec3(1.f, 1.f, 1.f));
	//Application::GetInstance()->PerformRunCycle();
	
#pragma region TENT_MODEL

	//CC_INFO("Tent");
	Application::GetInstance()->UpdateLoadingScene("Initializing tent.");
	//meshList_[TENT_MESH] = MeshBuilder::GenerateOBJMTL("Tent", "Assets/Tent/Obj/tent.obj", "Assets/Tent/Obj/tent.mtl");
	std::shared_ptr<GameObject> tent = CreateGameObject();
	tent->AddComponent<ModelRenderer>()->LoadModel("Assets/Tent/Obj/tent.obj");
	tent->GetTransform()->SetPosition({ 120, terrain->GetHeight(120, -50) + 7.1f, -50 });
	tent->GetTransform()->SetScale(glm::vec3(17));
	auto tent_collider = tent->AddComponent<BoxCollider>();
	tent_collider->Initialize();
	tent_collider->SetSize({ 78, 200, 78 });
#pragma endregion 

#pragma region BASKETS
	Application::GetInstance()->UpdateLoadingScene("Initializing basket and colliders.");
	std::shared_ptr<GameObject> basket = CreateGameObject();
	basket->AddComponent<ModelRenderer>()->LoadModel("Assets/MainScene/Basket/Basket.obj");
	basket->GetTransform()->SetPosition({ 0, terrain->GetHeight(0, -150) ,-150 });
	basket->GetTransform()->SetScale(glm::vec3(0.1f));

	std::shared_ptr<GameObject> basketLeft = CreateGameObject();
	basketLeft->GetTransform()->SetParent(basket->GetTransform());
	basketLeft->GetTransform()->SetPosition({ 40, 20, 0 });
	auto bcLeft = basketLeft->AddComponent<SphereCollider>();
	bcLeft->Initialize();
	bcLeft->SetRadius(10.0f);

	std::shared_ptr<GameObject> basketRight = CreateGameObject();
	basketRight->GetTransform()->SetParent(basket->GetTransform());
	basketRight->GetTransform()->SetPosition({ -40, 20, 0 });
	auto bcRight = basketRight->AddComponent<SphereCollider>();
	bcRight->Initialize();
	bcRight->SetRadius(10.0f);

	std::shared_ptr<GameObject> basketFront = CreateGameObject();
	basketFront->GetTransform()->SetParent(basket->GetTransform());
	basketFront->GetTransform()->SetPosition({ 0, 20, 40 });
	auto bcFront = basketFront->AddComponent<SphereCollider>();
	bcFront->Initialize();
	bcFront->SetRadius(10.0f);


	std::shared_ptr<GameObject> basketFloor = CreateGameObject();
	basketFloor->GetTransform()->SetParent(basket->GetTransform());
	basketFloor->GetTransform()->SetPosition({ 0, 40, 0 });
	auto bcFloor = basketFloor->AddComponent<BoxCollider>();
	bcFloor->Initialize();
	bcFloor->SetSize({ 13, 8, 13 });
	auto finishPoint = (!bumperCarSceneEnabled) ?
		basketFloor->AddComponent<FinishPoint>() : nullptr;

#pragma endregion

#pragma region BUMPER_SCENE_PORTAL
	meshList_[ARCADE_PORTAL] = MeshBuilder::GenerateQuad("Arcade Portal", glm::vec4(1.0f));
	mats_.push_back(new Material());
	mats_.back()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/MainScene/portal.png");
	meshList_[ARCADE_PORTAL]->materials.push_back(mats_.back());

	auto bumperPortalLight = CreateLight();
	bumperPortalLight->data->type = LightType::Point;
	bumperPortalLight->data->cos_cutoff = glm::radians(45.0f);
	bumperPortalLight->data->cos_inner = glm::radians(30.0f);
	bumperPortalLight->data->constant = 0.1f;
	bumperPortalLight->data->linear = 0.01f;
	bumperPortalLight->data->quadratic = 0.001f;
	bumperPortalLight->data->colour = { 102.f/255.f, 153.f/255.f, 204.f/255.f, 1.0f };
	bumperPortalLight->data->power = 0.0f;

	GameObject* bumperScenePortal = bumperPortalLight->GetGameObject();

	bumperScenePortal->AddComponent<UIRenderer>()->billboarded = true;
	bumperScenePortal->AddComponent<MeshFilter>()->SetMesh(meshList_[ARCADE_PORTAL]);
	bumperScenePortal->GetTransform()->SetPosition({ -50, terrain->GetHeight(110, 190) + 9.0f, -140 });
	bumperScenePortal->GetTransform()->SetScale({ 10, 15, 1 });
	bumperScenePortal->AddComponent<Portal>()->sceneName = "BumperCarScene";
	
	if (!bumperCarSceneEnabled)
	{
		finishPoint->BindObject(bumperScenePortal);
		finishPoint->BindConfetti(confetti.get());
	}

	auto plp = CreateLight();
	plp->data->type = LightType::Point;
	plp->data->cos_cutoff = glm::radians(45.0f);
	plp->data->cos_inner = glm::radians(30.0f);
	plp->data->constant = 0.1f;
	plp->data->linear = 0.01f;
	plp->data->quadratic = 0.001f;
	plp->data->colour = { 102.f/255.f, 153.f/255.f, 204.f/255.f, 1.0f };
	plp->data->power = 1.0f;

	GameObject* plpo = plp->GetGameObject();

	plpo->AddComponent<UIRenderer>()->billboarded = true;
	plpo->AddComponent<MeshFilter>()->SetMesh(meshList_[ARCADE_PORTAL]);
	plpo->GetTransform()->SetPosition({ 251, terrain->GetHeight(251, -104) + 9.0f, -104 });
	plpo->GetTransform()->SetScale({ 10, 15, 1 });
	plpo->AddComponent<Portal>()->sceneName = "BottleThrowScene";
#pragma endregion

#pragma region FERRIS
	Application::GetInstance()->UpdateLoadingScene("Creating Ferris wheel.");
	std::shared_ptr<GameObject> ferris = CreateGameObject();
	ferris->AddComponent<ModelRenderer>()->LoadModel("Assets/MainScene/Ferris/wheel.obj");
	ferris->GetTransform()->SetEulerAngles({ 0, 20, 0 });
	ferris->GetTransform()->SetPosition({ -70, terrain->GetHeight(-70, -210) + 40, -210 });
	ferris->GetTransform()->SetScale(glm::vec3(26.0f));
#pragma endregion

#pragma region CAROUSEL_MODEL

	Application::GetInstance()->UpdateLoadingScene("Initializing Carousel.");
	meshList_[CAROUSEL_BASE_MESH] = MeshBuilder::GenerateOBJMTL("Carousel", "Assets/Carousel/Obj/CarouselBase2.obj", "Assets/Carousel/Obj/CarouselBase2.mtl");
	meshList_[CAROUSEL_TOP_MESH] = MeshBuilder::GenerateOBJMTL("Carousel", "Assets/Carousel/Obj/CarouselTop2.obj", "Assets/Carousel/Obj/CarouselTop2.mtl");

	auto carousel_texture = Texture::LoadTexture("Assets/Carousel/Img/CarouselDiffuse.png");
	if (meshList_[CAROUSEL_BASE_MESH])
		meshList_[CAROUSEL_BASE_MESH]->materials[0]->textureMaps[Material::DIFFUSE] = carousel_texture;

	if (meshList_[CAROUSEL_TOP_MESH])
		meshList_[CAROUSEL_TOP_MESH]->materials[0]->textureMaps[Material::DIFFUSE] = carousel_texture;

	for (int i = 1; i < 40; i++)
		meshList_[CAROUSEL_TOP_MESH]->materials[i]->textureMaps[Material::DIFFUSE] = carousel_texture;

	mats_.push_back(meshList_[CAROUSEL_BASE_MESH]->materials.front());
	mats_.push_back(meshList_[CAROUSEL_TOP_MESH]->materials.front());


	carouselBase_ = CreateGameObject();
	carouselBase_->GetTransform()->SetScale({ 2.f, 2.f, 2.f });
	carouselBase_->GetTransform()->SetPosition({ 80.f, terrain->GetHeight(80.f, 80.f), 80.f});
	carouselBase_->AddComponent<MeshRenderer>();
	carouselBase_->AddComponent<MeshFilter>()->SetMesh(meshList_[CAROUSEL_BASE_MESH]);
	auto carousel_sc = carouselBase_->AddComponent<SphereCollider>();
	carousel_sc->SetRadius(25.f);
	carousel_sc->SetOffset({0, 10, 0});

	std::shared_ptr<GameObject> carouselTop = CreateGameObject();
	carouselTop->GetTransform()->SetScale({ 1.f, 1.f, 1.f });
	carouselTop->GetTransform()->SetPosition({ -0.25f, 0, 0.25f});
	carouselTop->GetTransform()->SetParent(carouselBase_->GetTransform());
	carouselTop->AddComponent<MeshRenderer>();
	carouselTop->AddComponent<MeshFilter>()->SetMesh(meshList_[CAROUSEL_TOP_MESH]);
	carouselTop->AddComponent<CarouselMovement>();
	
#pragma endregion 


#pragma region ARCADE

	Application::GetInstance()->UpdateLoadingScene("Creating Arcade and portal.");
	std::shared_ptr<GameObject> arcadeObj = CreateGameObject();
	arcadeObj->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/arcade.obj");
	arcadeObj->GetTransform()->SetPosition({ 150, terrain->GetHeight(150, 200), 220 });
	arcadeObj->GetTransform()->SetEulerAngles({ 0, 210, 0 });
	arcadeObj->GetTransform()->SetScale({ 2, 2 ,2 });
	auto arcadeSC = arcadeObj->AddComponent<SphereCollider>();
	arcadeSC->SetRadius(25.f);
	arcadeSC->Initialize();
	arcadeSC->SetOffset({ 0, 0, -5 });


	auto portalLight = CreateLight();
	portalLight->data->type = LightType::Point;
	portalLight->data->cos_cutoff = glm::radians(45.0f);
	portalLight->data->cos_inner = glm::radians(30.0f);
	portalLight->data->constant = 0.1f;
	portalLight->data->linear = 0.01f;
	portalLight->data->quadratic = 0.001f;
	portalLight->data->colour = { 102.f/255.f, 153.f/255.f, 204.f/255.f, 1.0f };
	portalLight->data->power = 1.0f;

	GameObject* arcadePortalObj = portalLight->GetGameObject();

	arcadePortalObj->AddComponent<UIRenderer>()->billboarded = true;
	arcadePortalObj->AddComponent<MeshFilter>()->SetMesh(meshList_[ARCADE_PORTAL]);
	arcadePortalObj->GetTransform()->SetPosition({ 133, terrain->GetHeight(110, 190) + 10.2f, 190 });
	arcadePortalObj->GetTransform()->SetScale({ 10, 15, 1 });
	arcadePortalObj->AddComponent<Portal>()->sceneName = "ArcadeScene";

#pragma endregion


#pragma region STARTING_PLACE_ROCKS
	Application::GetInstance()->UpdateLoadingScene("Creating terrain rock.");
	std::shared_ptr<GameObject> rockObj = CreateGameObject();
	ModelRenderer* largeRockModel = rockObj->AddComponent<ModelRenderer>();
	largeRockModel->LoadModel("Assets/MainScene/Rock/LargeCaveRock_Obj.obj");
	rockObj->GetTransform()->SetPosition({ -100, terrain->GetHeight(-100, 125) + 16, 125 });
	rockObj->GetTransform()->SetEulerAngles({ 0, 20, 0 });
	rockObj->GetTransform()->SetScale({ 2, 2 ,2 });
	Application::GetInstance()->UpdateLoadingScene("Creating terrain colliders.");
	BoxCollider* rockBc1 = rockObj->AddComponent<BoxCollider>();
	rockBc1->Initialize();
	rockBc1->SetSize({ 110, 200, 50 });

	std::shared_ptr<GameObject> extraCollider = CreateGameObject();
	extraCollider->GetTransform()->SetPosition({ -55, terrain->GetHeight(-55, 160) + 16, 160 });
	BoxCollider* rockBcExtra1 = extraCollider->AddComponent<BoxCollider>();
	rockBcExtra1->Initialize();
	rockBcExtra1->SetSize({ 120, 200, 75 });

	std::shared_ptr<GameObject> extraCollider2 = CreateGameObject();
	extraCollider2->GetTransform()->SetPosition({ -140, terrain->GetHeight(-140, 80) + 16, 80 });
	BoxCollider* rockBcExtra2 = extraCollider2->AddComponent<BoxCollider>();
	rockBcExtra2->Initialize();
	rockBcExtra2->SetSize({ 120, 200, 120 });

	std::shared_ptr<GameObject> extraCollider3 = CreateGameObject();
	extraCollider3->GetTransform()->SetPosition({ -240, terrain->GetHeight(-240, 80) + 16, 80 });
	BoxCollider* rockBcExtra3 = extraCollider3->AddComponent<BoxCollider>();
	rockBcExtra3->Initialize();
	rockBcExtra3->SetSize({ 180, 200, 120 });

	std::shared_ptr<GameObject> extraCollider4 = CreateGameObject();
	extraCollider4->GetTransform()->SetPosition({ -18, terrain->GetHeight(-18, 80) + 16, 88 });
	BoxCollider* rockBcExtra4 = extraCollider4->AddComponent<BoxCollider>();
	rockBcExtra4->Initialize();
	rockBcExtra4->SetSize({ 40, 200, 70 });

	std::shared_ptr<GameObject> extraCollider5 = CreateGameObject();
	extraCollider5->GetTransform()->SetPosition({ -70, terrain->GetHeight(-70, 50) + 16, 50 });
	BoxCollider* rockBcExtra5 = extraCollider5->AddComponent<BoxCollider>();
	rockBcExtra5->Initialize();
	rockBcExtra5->SetSize({ 120, 200, 60 });

	std::shared_ptr<GameObject> extraCollider6 = CreateGameObject();
	extraCollider6->GetTransform()->SetPosition({ -159, terrain->GetHeight(-159, -12.5f) - 7, -12.5f });
	SphereCollider* rockBcExtra6 = extraCollider6->AddComponent<SphereCollider>();
	rockBcExtra6->Initialize();
	rockBcExtra6->SetRadius(22.f);

	// Boundary collider near spawn
	std::shared_ptr<GameObject> extraCollider7 = CreateGameObject();
	extraCollider7->GetTransform()->SetPosition({ -263, 159, 0 });
	BoxCollider* rockBcExtra7 = extraCollider7->AddComponent<BoxCollider>();
	rockBcExtra7->Initialize();
	rockBcExtra7->SetSize({ 40, 800, 800 });

	std::shared_ptr<GameObject> extraCollider8 = CreateGameObject();
	extraCollider8->GetTransform()->SetPosition({ 0, 159, 295 });
	BoxCollider* rockBcExtra8 = extraCollider8->AddComponent<BoxCollider>();
	rockBcExtra8->Initialize();
	rockBcExtra8->SetSize({ 800, 800, 40 });

	std::shared_ptr<GameObject> extraCollider9 = CreateGameObject();
	extraCollider9->GetTransform()->SetPosition({ 0, 159, -175 });
	BoxCollider* rockBcExtra9 = extraCollider9->AddComponent<BoxCollider>();
	rockBcExtra9->Initialize();
	rockBcExtra9->SetSize({ 800, 800, 40 });

	std::shared_ptr<GameObject> extraCollider10 = CreateGameObject();
	extraCollider10->GetTransform()->SetPosition({ 337, 159, -90 });
	BoxCollider* rockBcExtra10 = extraCollider10->AddComponent<BoxCollider>();
	rockBcExtra10->Initialize();
	rockBcExtra10->SetSize({ 40, 800, 400 });

	std::shared_ptr<GameObject> extraCollider11 = CreateGameObject();
	extraCollider11->GetTransform()->SetPosition({ 285, 159, 105 });
	BoxCollider* rockBcExtra11 = extraCollider11->AddComponent<BoxCollider>();
	rockBcExtra11->Initialize();
	rockBcExtra11->SetSize({ 140, 800, 40 });

	std::shared_ptr<GameObject> extraCollider12 = CreateGameObject();
	extraCollider12->GetTransform()->SetPosition({ 225, 159, 295 });
	BoxCollider* rockBcExtra12 = extraCollider12->AddComponent<BoxCollider>();
	rockBcExtra12->Initialize();
	rockBcExtra12->SetSize({ 40, 800, 400 });

	Application::GetInstance()->UpdateLoadingScene("Creating more terrain rocks.");
	std::shared_ptr<GameObject> rockObj2 = CreateGameObject();
	rockObj2->AddComponent<ModelRenderer>()->ReferenceModelRenderer(largeRockModel);
	rockObj2->GetTransform()->SetPosition({ -220, terrain->GetHeight(-243, 77) + 32, 90 });
	rockObj2->GetTransform()->SetEulerAngles({ 0, 50, 0 });
	rockObj2->GetTransform()->SetScale({ 2, 2 ,2 });

	std::shared_ptr<GameObject> rockArch = CreateGameObject();
	rockArch->AddComponent<ModelRenderer>()->LoadModel("Assets/MainScene/ArchRock/SmallArch_Obj.obj");
	rockArch->GetTransform()->SetPosition({ -68, terrain->GetHeight(-68, 245) + 30, 245});
	rockArch->GetTransform()->SetEulerAngles({ 0, -64, 0 });
	rockArch->GetTransform()->SetScale({ 2, 2 ,2 });
#pragma endregion

	Application::GetInstance()->UpdateLoadingScene("Creating green crystals.");

#pragma region GREEN_CRYSTAL_LIGHTS

	std::vector<float> green_gem_powers
	{
		0.25f,
		0.25f,
		1.0f,
		1.0f,
		2.0f,
		0.25f,
		1.0f,
	};

	auto l = CreateLight();

	l->data->type = LightType::Point;
	l->data->cos_cutoff = glm::radians(45.0f);
	l->data->cos_inner = glm::radians(30.0f);
	l->data->constant = 0.1f;
	l->data->linear = 0.01f;
	l->data->quadratic = 0.001f;
	l->data->colour = { 0.1f, 1.0f, 0.1f, 1.0f };
	l->data->power = green_gem_powers.front();

	std::vector<glm::vec3> green_gem_positions
	{
		{ -220, terrain->GetHeight(-220, 133) + 5.f, 133 },
		{ -120, terrain->GetHeight(-210, 123) + 2.f, 158 },
		{ -80, terrain->GetHeight(-210, 123) + 7.f, 300 },
		{ 250, terrain->GetHeight(250, -220) + 7.f, -220 },
		{ -320, terrain->GetHeight(-320, -132) + 4.f, -132 },
		{ 120, terrain->GetHeight(120, -50) + 1.5f, -50 },
		{ 40, terrain->GetHeight(40, 100), 100 },
	};
	std::vector<glm::vec3> green_gem_rotations 
	{
		{10, 20, 0},
		{},
		{10, 50, 0},
		{10, -20, 0},
		{10, 0, 10},
		{},
		{10, 20, 0},
	};

	std::vector<glm::vec3> green_gem_scales 
	{
		{1,1,1},
		{1,1,1},
		{5,5,5},
		{5,5,5},
		{10,10,10},
		{1,1,1},
		{1,1,1},
	};


	GameObject* gemBase = l->GetGameObject();
	gemBase->GetTransform()->SetPosition(green_gem_positions.front());
	gemBase->GetTransform()->SetEulerAngles(green_gem_rotations.front());
	gemBase->GetTransform()->SetScale(green_gem_scales.front());



	std::shared_ptr<GameObject> gem = CreateGameObject();
	ModelRenderer* gemModel = gem->AddComponent<ModelRenderer>(); 
	gemModel->LoadModel("Assets/MainScene/Gems/OBJ/GemShard.obj");
	gem->GetTransform()->SetPosition({ -0.15f, 0.05f, -0.15f });
	gem->GetTransform()->SetScale(glm::vec3(2.f/3.f));
	gem->GetTransform()->SetEulerAngles({-10, 100, 0});

	std::shared_ptr<GameObject> gem2 = CreateGameObject();
	gem2->AddComponent<ModelRenderer>()->ReferenceModelRenderer(gemModel);
	gem2->GetTransform()->SetPosition({ 0, -2.35f, 0 });
	gem2->GetTransform()->SetScale({6, 6, 6});
	gem2->GetTransform()->SetParent(gemBase->GetTransform());

	std::shared_ptr<GameObject> gem3 = CreateGameObject();
	gem3->AddComponent<ModelRenderer>()->ReferenceModelRenderer(gemModel);
	gem3->GetTransform()->SetPosition({ 0.15f, 0.05f, 0.15f });
	gem3->GetTransform()->SetScale(glm::vec3(2.f/3.f));
	gem3->GetTransform()->SetEulerAngles({10, 130, 0});

	gem->GetTransform()->SetParent(gem2->GetTransform());
	gem3->GetTransform()->SetParent(gem2->GetTransform());

	for (int i = 1; i < green_gem_positions.size(); ++i)
	{
		auto light = CreateLight();

		light->data->type = LightType::Point;
		light->data->cos_cutoff = glm::radians(45.0f);
		light->data->cos_inner = glm::radians(30.0f);
		light->data->constant = 0.1f;
		light->data->linear = 0.01f;
		light->data->quadratic = 0.001f;
		light->data->colour = { 0.1f, 1.0f, 0.1f, 1.0f };
		light->data->power = green_gem_powers[i];

		GameObject* gBase = light->GetGameObject();
		gBase->GetTransform()->SetPosition(green_gem_positions[i]);
		gBase->GetTransform()->SetEulerAngles(green_gem_rotations[i]);
		gBase->GetTransform()->SetScale(green_gem_scales[i]);

		std::shared_ptr<GameObject> g = CreateGameObject();
		g->AddComponent<ModelRenderer>()->ReferenceModelRenderer(gemModel);
		g->GetTransform()->SetPosition({ -0.15f, 0.05f, -0.15f });
		g->GetTransform()->SetScale(glm::vec3(2.f/3.f));
		g->GetTransform()->SetEulerAngles({-10, 100, 0});

		std::shared_ptr<GameObject> g2 = CreateGameObject();
		g2->AddComponent<ModelRenderer>()->ReferenceModelRenderer(gemModel);
		g2->GetTransform()->SetPosition({ 0, -2.35f, 0 });
		g2->GetTransform()->SetScale({6, 6, 6});
		g2->GetTransform()->SetParent(gBase->GetTransform());

		std::shared_ptr<GameObject> g3 = CreateGameObject();
		g3->AddComponent<ModelRenderer>()->ReferenceModelRenderer(gemModel);
		g3->GetTransform()->SetPosition({ 0.15f, 0.05f, 0.15f });
		g3->GetTransform()->SetScale(glm::vec3(2.f/3.f));
		g3->GetTransform()->SetEulerAngles({10, 130, 0});

		g->GetTransform()->SetParent(g2->GetTransform());
		g3->GetTransform()->SetParent(g2->GetTransform());

		if (i == 5 || i == 6)
		{
			gBase->AddComponent<RigidBody>()->SetMass(5.0f);
			auto sc = gBase->AddComponent<SphereCollider>();
			sc->SetRadius(2.5f);
			sc->Initialize();
			gBase->AddComponent<ThrowableRock>();
			g->AddComponent<DebugMovement>();
			gBase->tag = "Green";
		}
	}

#pragma endregion

	Application::GetInstance()->UpdateLoadingScene("Creating blue crystals.");

#pragma region BLUE_CRYSTAL_LIGHTS
	std::vector<float> blue_gem_powers
	{
		1.0f,
		0.25f,
		1.0f,
		0.5f,
		2.0f,
		0.25f,
	};

	auto bl = CreateLight();

	bl->data->type = LightType::Point;
	bl->data->cos_cutoff = glm::radians(45.0f);
	bl->data->cos_inner = glm::radians(30.0f);
	bl->data->constant = 0.1f;
	bl->data->linear = 0.01f;
	bl->data->quadratic = 0.001f;
	bl->data->colour = { 102.f/255.f, 153.f/255.f, 204.f/255.f, 1.0f };
	bl->data->power = blue_gem_powers.front();


	std::vector<glm::vec3> blue_gem_positions
	{
		{ -50, terrain->GetHeight(-70, 170) + 5.f, 185 },
		{ -185, terrain->GetHeight(-185, 200) + 0.4f, 200 },
		{ -5, terrain->GetHeight(-5, 100) + 5.f, 100 },
		{ 80, terrain->GetHeight(100, -230) + 4.f, -230 },
		{ -266, terrain->GetHeight(-266, 10) + 4.f, 10 },
		{ 200, terrain->GetHeight(200, 50) + 2.f, 50 },
	};
	std::vector<glm::vec3> blue_gem_rotations 
	{
		{10, 40, 0},
		{},
		{0, -20, -15},
		{10, -20, 0},
		{0, 0, 30},
		{},
	};

	std::vector<glm::vec3> blue_gem_scales 
	{
		{5,5,5},
		{1,1,1},
		{5,5,5},
		{3,3,3},
		{10,10,10},
		{1,1,1},
	};


	GameObject* blueGemBase = bl->GetGameObject();
	blueGemBase->GetTransform()->SetPosition(blue_gem_positions.front());
	blueGemBase->GetTransform()->SetEulerAngles(blue_gem_rotations.front());
	blueGemBase->GetTransform()->SetScale(blue_gem_scales.front());



	std::shared_ptr<GameObject> blueGem = CreateGameObject();
	ModelRenderer* blueGemModel = blueGem->AddComponent<ModelRenderer>(); 
	blueGemModel->LoadModel("Assets/MainScene/Gems/OBJ/BlueGemShard.obj");
	blueGem->GetTransform()->SetPosition({ -0.15f, 0.05f, -0.15f });
	blueGem->GetTransform()->SetScale(glm::vec3(2.f/3.f));
	blueGem->GetTransform()->SetEulerAngles({-10, 100, 0});

	std::shared_ptr<GameObject> blueGem2 = CreateGameObject();
	blueGem2->AddComponent<ModelRenderer>()->ReferenceModelRenderer(blueGemModel);
	blueGem2->GetTransform()->SetPosition({ 0, -2.35f, 0 });
	blueGem2->GetTransform()->SetScale({6, 6, 6});
	blueGem2->GetTransform()->SetParent(blueGemBase->GetTransform());

	std::shared_ptr<GameObject> blueGem3 = CreateGameObject();
	blueGem3->AddComponent<ModelRenderer>()->ReferenceModelRenderer(blueGemModel);
	blueGem3->GetTransform()->SetPosition({ 0.15f, 0.05f, 0.15f });
	blueGem3->GetTransform()->SetScale(glm::vec3(2.f/3.f));
	blueGem3->GetTransform()->SetEulerAngles({10, 130, 0});

	blueGem->GetTransform()->SetParent(blueGem2->GetTransform());
	blueGem3->GetTransform()->SetParent(blueGem2->GetTransform());

	for (int i = 1; i < blue_gem_positions.size(); ++i)
	{
		auto light = CreateLight();

		light->data->type = LightType::Point;
		light->data->cos_cutoff = glm::radians(45.0f);
		light->data->cos_inner = glm::radians(30.0f);
		light->data->constant = 0.1f;
		light->data->linear = 0.01f;
		light->data->quadratic = 0.001f;
		light->data->colour = { 102.f/255.f, 153.f/255.f, 204.f/255.f, 1.0f };
		light->data->power = blue_gem_powers[i];

		GameObject* gBase = light->GetGameObject();
		gBase->GetTransform()->SetPosition(blue_gem_positions[i]);
		gBase->GetTransform()->SetEulerAngles(blue_gem_rotations[i]);
		gBase->GetTransform()->SetScale(blue_gem_scales[i]);

		std::shared_ptr<GameObject> g = CreateGameObject();
		g->AddComponent<ModelRenderer>()->ReferenceModelRenderer(blueGemModel);
		g->GetTransform()->SetPosition({ -0.15f, 0.05f, -0.15f });
		g->GetTransform()->SetScale(glm::vec3(2.f/3.f));
		g->GetTransform()->SetEulerAngles({-10, 100, 0});

		std::shared_ptr<GameObject> g2 = CreateGameObject();
		g2->AddComponent<ModelRenderer>()->ReferenceModelRenderer(blueGemModel);
		g2->GetTransform()->SetPosition({ 0, -2.35f, 0 });
		g2->GetTransform()->SetScale({6, 6, 6});
		g2->GetTransform()->SetParent(gBase->GetTransform());

		std::shared_ptr<GameObject> g3 = CreateGameObject();
		g3->AddComponent<ModelRenderer>()->ReferenceModelRenderer(blueGemModel);
		g3->GetTransform()->SetPosition({ 0.15f, 0.05f, 0.15f });
		g3->GetTransform()->SetScale(glm::vec3(2.f/3.f));
		g3->GetTransform()->SetEulerAngles({10, 130, 0});

		g->GetTransform()->SetParent(g2->GetTransform());
		g3->GetTransform()->SetParent(g2->GetTransform());
		if (i == 5)
		{
			gBase->AddComponent<RigidBody>()->SetMass(5.0f);
			auto sc = gBase->AddComponent<SphereCollider>();
			sc->SetRadius(2.5f);
			sc->Initialize();
			gBase->AddComponent<ThrowableRock>();
			g->AddComponent<DebugMovement>();
			gBase->tag = "Blue";
		}

	}
#pragma endregion

	Application::GetInstance()->UpdateLoadingScene("Creating red crystals.");

#pragma region RED_CRYSTAL_LIGHTS
	std::vector<float> red_gem_powers 
	{
		0.25f,
		0.5f,
		0.25f,
		0.8f,
		1.7f,
	};

	auto rl = CreateLight();

	rl->data->type = LightType::Point;
	rl->data->cos_cutoff = glm::radians(45.0f);
	rl->data->cos_inner = glm::radians(30.0f);
	rl->data->constant = 0.1f;
	rl->data->linear = 0.01f;
	rl->data->quadratic = 0.001f;
	rl->data->colour = { 1.f, 0.f, 0.f, 1.0f };
	rl->data->power = red_gem_powers.front();

	std::vector<glm::vec3> red_gem_positions
	{
		{ -30, terrain->GetHeight(-30, 50) + 0.5f, 50 },
		{ -210, terrain->GetHeight(-210, 123) + 37.f, 123 },
		{ -10, terrain->GetHeight(-10, 20) + 1.0f, 20 },
		{ 40, terrain->GetHeight(40, -280) + 2.f, -280 },
		{ -283, terrain->GetHeight(-283, -239) + 2.f, -218 },
	};
	std::vector<glm::vec3> red_gem_rotations 
	{
		{},
		{-10, -20, 0},
		{},
		{20, 0, 0},
		{0, 10, 20},
	};

	std::vector<glm::vec3> red_gem_scales 
	{
		{1,1,1},
		{1.5,1.5,1.5},
		{1,1,1},
		{3,3,3},
		{9,9,9},
	};


	GameObject* redGemBase = rl->GetGameObject();
	redGemBase->GetTransform()->SetPosition(red_gem_positions.front());
	redGemBase->GetTransform()->SetEulerAngles(red_gem_rotations.front());
	redGemBase->GetTransform()->SetScale(red_gem_scales.front());

	std::shared_ptr<GameObject> redGem = CreateGameObject();
	ModelRenderer* redGemModel = redGem->AddComponent<ModelRenderer>(); 
	redGemModel->LoadModel("Assets/MainScene/Gems/OBJ/RedGemShard.obj");
	redGem->GetTransform()->SetPosition({ -0.15f, 0.05f, -0.15f });
	redGem->GetTransform()->SetScale(glm::vec3(2.f/3.f));
	redGem->GetTransform()->SetEulerAngles({-10, 100, 0});

	std::shared_ptr<GameObject> redGem2 = CreateGameObject();
	redGem2->AddComponent<ModelRenderer>()->ReferenceModelRenderer(redGemModel);
	redGem2->GetTransform()->SetPosition({ 0, -2.35f, 0 });
	redGem2->GetTransform()->SetScale({6, 6, 6});
	redGem2->GetTransform()->SetParent(redGemBase->GetTransform());

	std::shared_ptr<GameObject> redGem3 = CreateGameObject();
	redGem3->AddComponent<ModelRenderer>()->ReferenceModelRenderer(redGemModel);
	redGem3->GetTransform()->SetPosition({ 0.15f, 0.05f, 0.15f });
	redGem3->GetTransform()->SetScale(glm::vec3(2.f/3.f));
	redGem3->GetTransform()->SetEulerAngles({10, 130, 0});

	redGem->GetTransform()->SetParent(redGem2->GetTransform());
	redGem3->GetTransform()->SetParent(redGem2->GetTransform());

	for (int i = 1; i < red_gem_positions.size(); ++i)
	{
		auto light = CreateLight();

		light->data->type = LightType::Point;
		light->data->cos_cutoff = glm::radians(45.0f);
		light->data->cos_inner = glm::radians(30.0f);
		light->data->constant = 0.1f;
		light->data->linear = 0.01f;
		light->data->quadratic = 0.001f;
		light->data->colour = { 1.0f, 0, 0, 1.0f };
		light->data->power = red_gem_powers[i];

		GameObject* gBase = light->GetGameObject();
		gBase->GetTransform()->SetPosition(red_gem_positions[i]);
		gBase->GetTransform()->SetEulerAngles(red_gem_rotations[i]);
		gBase->GetTransform()->SetScale(red_gem_scales[i]);

		std::shared_ptr<GameObject> g = CreateGameObject();
		g->AddComponent<ModelRenderer>()->ReferenceModelRenderer(redGemModel);
		g->GetTransform()->SetPosition({ -0.15f, 0.05f, -0.15f });
		g->GetTransform()->SetScale(glm::vec3(2.f/3.f));
		g->GetTransform()->SetEulerAngles({-10, 100, 0});

		std::shared_ptr<GameObject> g2 = CreateGameObject();
		g2->AddComponent<ModelRenderer>()->ReferenceModelRenderer(redGemModel);
		g2->GetTransform()->SetPosition({ 0, -2.35f, 0 });
		g2->GetTransform()->SetScale({6, 6, 6});
		g2->GetTransform()->SetParent(gBase->GetTransform());

		std::shared_ptr<GameObject> g3 = CreateGameObject();
		g3->AddComponent<ModelRenderer>()->ReferenceModelRenderer(redGemModel);
		g3->GetTransform()->SetPosition({ 0.15f, 0.05f, 0.15f });
		g3->GetTransform()->SetScale(glm::vec3(2.f/3.f));
		g3->GetTransform()->SetEulerAngles({10, 130, 0});

		g->GetTransform()->SetParent(g2->GetTransform());
		g3->GetTransform()->SetParent(g2->GetTransform());

		if (i == 2)
		{
			gBase->AddComponent<RigidBody>()->SetMass(5.0f);
			auto sc = gBase->AddComponent<SphereCollider>();
			sc->SetRadius(2.5f);
			sc->Initialize();
			gBase->AddComponent<ThrowableRock>();
			g->AddComponent<DebugMovement>();
			gBase->tag = "Red";
		}
	}
#pragma endregion

	Application::GetInstance()->UpdateLoadingScene("Creating purple crystals.");

#pragma region PURPLE_CRYSTAL_LIGHTS 
	std::vector<float> purple_gem_powers 
	{
		0.25f,
	};

	Light* pl = CreateLight();

	pl->data->type = LightType::Point;
	pl->data->cos_cutoff = glm::radians(45.0f);
	pl->data->cos_inner = glm::radians(30.0f);
	pl->data->constant = 0.1f;
	pl->data->linear = 0.01f;
	pl->data->quadratic = 0.001f;
	pl->data->colour = { 148.f/255.f, 0.f, 211.f/255.f, 1.0f };
	pl->data->power = purple_gem_powers.front();

	std::vector<glm::vec3> purple_gem_positions
	{
		{ 0, terrain->GetHeight(0, 0) + 2.0f, 0 },
	};
	std::vector<glm::vec3> purple_gem_rotations 
	{
		{},
	};

	std::vector<glm::vec3> purple_gem_scales 
	{
		{1,1,1},
	};

	GameObject* purpleGemBase = pl->GetGameObject();
	//purpleGemBase->GetTransform()->SetPosition(purple_gem_positions.front());
	purpleGemBase->GetTransform()->SetPosition({0, 10.8f, -10.5f});
	purpleGemBase->GetTransform()->SetEulerAngles({0, 0, 0});
	//purpleGemBase->GetTransform()->SetScale(purple_gem_scales.front());
	purpleGemBase->GetTransform()->SetParent(carouselTop->GetTransform());

	std::shared_ptr<GameObject> purpleGem = CreateGameObject();
	ModelRenderer* purpleGemModel = purpleGem->AddComponent<ModelRenderer>(); 
	purpleGemModel->LoadModel("Assets/MainScene/Gems/OBJ/Amethyst.obj");
	//purpleGem->GetTransform()->SetPosition({ 0, -2.35f, 0 });
	purpleGem->GetTransform()->SetScale({1.5f, 1.5f, 1.5f});
	purpleGem->GetTransform()->SetParent(purpleGemBase->GetTransform());

	//for (int i = 1; i < purple_gem_positions.size(); ++i)
	//{
		auto purp_light = CreateLight();

		purp_light->data->type = LightType::Point;
		purp_light->data->cos_cutoff = glm::radians(45.0f);
		purp_light->data->cos_inner = glm::radians(30.0f);
		purp_light->data->constant = 0.1f;
		purp_light->data->linear = 0.01f;
		purp_light->data->quadratic = 0.001f;
		purp_light->data->colour = { 148.f/255.f, 0.f, 211.f/255.f, 1.0f };
		purp_light->data->power = 0.25f;

		GameObject* purp_gBase = purp_light->GetGameObject();
		//gBase->GetTransform()->SetPosition(purple_gem_positions[i]);
		purp_gBase->GetTransform()->SetPosition({0, 10.8f, 10.5f});
		purp_gBase->GetTransform()->SetEulerAngles({0, 0, 0});
		//purp_gBase->GetTransform()->SetScale();
		purp_gBase->GetTransform()->SetParent(carouselTop->GetTransform());

		std::shared_ptr<GameObject> g = CreateGameObject();
		g->AddComponent<ModelRenderer>()->ReferenceModelRenderer(purpleGemModel);
		//g->GetTransform()->SetPosition({ 0, 0.f, 0 });
		g->GetTransform()->SetScale(glm::vec3(1.5f));
		g->GetTransform()->SetParent(purp_gBase->GetTransform());
	//}
#pragma endregion

#pragma region EXTRA_DETAIL
	Application::GetInstance()->UpdateLoadingScene("Loading extra details");
	std::shared_ptr<GameObject> yellow_stall = CreateGameObject();
	yellow_stall->AddComponent<ModelRenderer>()->LoadModel("Assets/Stall/Obj/Stall.obj");
	yellow_stall->GetTransform()->SetPosition({ 100, terrain->GetHeight(80, 280) + 0.0f, 280 });
	yellow_stall->GetTransform()->SetEulerAngles(glm::vec3(0, 210, 0));
	yellow_stall->GetTransform()->SetScale(glm::vec3(1.75f));

	std::shared_ptr<GameObject> red_stall = CreateGameObject();
	red_stall->AddComponent<ModelRenderer>()->LoadModel("Assets/Stall/Obj/RedStall.obj");
	red_stall->GetTransform()->SetPosition({ 210, terrain->GetHeight(210, 250) + 0.0f, 250 });
	red_stall->GetTransform()->SetEulerAngles(glm::vec3(0, 230, 0));
	red_stall->GetTransform()->SetScale(glm::vec3(1.75f));

	std::shared_ptr<GameObject> blue_stall = CreateGameObject();
	blue_stall->AddComponent<ModelRenderer>()->LoadModel("Assets/Stall/Obj/BlueStall.obj");
	blue_stall->GetTransform()->SetPosition({ 275, terrain->GetHeight(275, -101) + 0.0f, -101 });
	blue_stall->GetTransform()->SetEulerAngles(glm::vec3(0, -90, 0));
	blue_stall->GetTransform()->SetScale(glm::vec3(1.75f));

#pragma endregion EXTRA_DETAIL
	sceneCamera_ = cameras_[0];
	fpsCounter_->SetText("FPS: 0");
}

void MainScene::Update(double dt)
{
	constexpr double fps_update_time = 1.0;
	static double timer = 0.0;
	timer += dt;
	if (timer > fps_update_time)
	{
		fpsCounter_->SetText("FPS: " + std::to_string(static_cast<int>(1.0 / dt)));
		timer -= fps_update_time;
	}

	if (Input::GetInstance()->IsKeyPressed('N') && !isChangingCamera_)
	{
		//static bool player = true;
		static int index = 1;
		SwitchCamera(cameras_[index]);
		++index;
		if (index == NUM_CAM)
			index = 0;
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

	static float rotation = 0;
	rotation += 0.005f;
	if (rotation > 360)
		rotation -= 360.f;

	constexpr glm::vec3 rotation_direction = glm::vec3(Util::sqrt(2), Util::sqrt(2), 0);
	skybox_->SetRotation(glm::rotate(glm::mat4(1.0f), glm::radians(rotation), rotation_direction));
}

void MainScene::Render()
{
	if (isChangingScene_)
	{
		Application::GetInstance()->LoadScene(sceneNameToChangeTo_);
		isChangingScene_ = false;
		return;
	}
	if (Input::GetInstance()->IsKeyDown('B'))
	{
		Application::GetInstance()->QueueSceneLoad("BumperCarScene");
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

void MainScene::Exit()
{
	//for (auto& i : objects_)
	//	if (i)
	//	{
	//		delete i;
	//		i = nullptr;
	//	}
	TextureManager::GetInstance()->ClearTextures();

	//delete fpsCounter_;
	//fpsCounter_ = nullptr;

	//delete dialogueText_;
	//dialogueText_ = nullptr;


	if (skybox_)
	{
		delete skybox_;
		skybox_ = nullptr;
	}


	delete inconsolataFont_;
	inconsolataFont_ = nullptr;

	//delete playerModel_.front()->materials.front()->textureMaps[Material::DIFFUSE];

	CC_TRACE("Deleting mesh list.");
	for (auto& i : meshList_)
	{ 
		//if (!i)
		//	continue;
		//for (const auto& j : i->materials)
		//{
		//	delete j;
		//}
		delete i;
		i = nullptr;
	}
	
	CC_TRACE("Deleting player model.");
	for (auto& i : playerModel_)
	{ 
		//if (!i)
		//	continue;
		//for (const auto& j : i->materials)
		//{
		//	delete j;
		//}
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
	//for (int i = 0; i < NUM_PLAYER_PARTS; ++i)
	//{
	//	auto& j = playerModel_[i];
	//	if (!j || !j->materials.empty()) continue;
	//	j->materials.front()->textureMaps[Material::DIFFUSE] = nullptr;
	//	delete j;
	//	j = nullptr;
	//}
}



Light* MainScene::CreateLight()
{
	std::shared_ptr<GameObject> temp = CreateGameObject();
	auto temp_light = temp->AddComponent<Light>(); 	
	lights_.emplace_back(temp_light->data);
	//lights_[numLights_] = temp_light->data;
	//++numLights_;
	return temp_light;
}

void MainScene::CameraInterp()
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

void MainScene::SwitchCamera(Camera* newCamera)
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

MainScene::~MainScene()
{
}
