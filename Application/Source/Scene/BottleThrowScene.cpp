#pragma region headers
#include "Components/ModelRenderer.h"
#include "Components/Portal.h"
#include "BottleThrowScene.h"
#include "KeyboardController.h"
#include "Utils/Util.h"
#include "Utils/Input.h"
#include "Physics/ColliderManager.h"
#include <Windows.h>
#include "Utils/LoadOBJ.h"
#include "Utils/Input.h"
#include "Components/Terrain.h"
#include "Components/StallFloor.h"


#include "Renderer/MeshBuilder.h"
#include "Components/Text.h"

#include <iostream>

#include "Renderer/SceneRenderer.h"
#include "Core/Application.h"

#include "GameObject.h"

// Components
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
#include "Components/BoxCollider.h"
#include "Components/PlayerController.h"
#include "Components/PlayerMovement.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#pragma endregion
BottleThrowScene::BottleThrowScene()
	: shaderLib_()
	, isChangingCamera_(false)
{
	applicationRefreshes_ = 11;
}

void BottleThrowScene::Init()
{
#pragma region shader and camera stuff
	Application::GetInstance()->UpdateLoadingScene("Setting shaders");
	shaderLib_ = ShaderLibrary::GetInstance();

	Shader::MultitexturedShader = shaderLib_->Get("Terrain Shader");
	if (Shader::MultitexturedShader)
		Shader::MultitexturedShader->SetFlags(ShaderFlag::MULTI_TEXTURING);

	auto envShader = shaderLib_->Get("Environment Map");
	envShader->SetFlags(ShaderFlag::ENVIRONMENT_MAPPED);

	shaderLib_->RegisterUniformBuffer("Material");
	shaderLib_->RegisterUniformBuffer("LightBlock");

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

	//auto tpc = CreateGameObject();
	//_cameras[2] = tpc->AddComponent<Camera>();
	//auto pcc = tpc->AddComponent<PanningCameraController>();

	cameras_[1]->SetActive(false);
	cameras_[1]->projection = projectionMatrix;
#pragma endregion
#pragma region playermodelgenerate
	Application::GetInstance()->UpdateLoadingScene("Loading Player model");
	playerModel_[HEAD] = MeshBuilder::GenerateOBJMTL("Head", "Assets/player/head.obj", "Assets/player/head.mtl");
	playerModel_[LEFT_ARM] = MeshBuilder::GenerateOBJMTL("Left arm", "Assets/player/leftarm.obj", "Assets/player/leftarm.mtl");
	playerModel_[RIGHT_ARM] = MeshBuilder::GenerateOBJMTL("Right arm", "Assets/player/rightarm.obj", "Assets/player/rightarm.mtl");
	playerModel_[LEFT_LEG] = MeshBuilder::GenerateOBJMTL("Left leg", "Assets/player/leftleg.obj", "Assets/player/leftleg.mtl");
	playerModel_[RIGHT_LEG] = MeshBuilder::GenerateOBJMTL("Right leg", "Assets/player/rightleg.obj", "Assets/player/rightleg.mtl");
	playerModel_[TORSO] = MeshBuilder::GenerateOBJMTL("Torso", "Assets/player/torso.obj", "Assets/player/torso.mtl");


	for (int i = 0; i < NUM_PLAYER_PARTS; ++i)
	{
		const auto& j = playerModel_[i];
		if (!j) continue;
		if (j->materials.empty()) continue;
		j->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/player/texture.png");
	}
	for (const auto& i : playerModel_)
	{
		for (const auto& j : i->materials)
		{
			mats_.push_back(j);
		}
	}
#pragma endregion
#pragma region cubematerial and white
	Material* cubeMaterial = new Material(
		{ 0.3f, 0.3f, 0.3f, 1.0f },
		{ 0.2f, 0.2f, 0.2f, 1.0f },
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

	//static std::shared_ptr<GameObject> cube = CreateGameObject();
	//cube->AddComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEOMETRY_TYPE::GEO_BALL]));
	//cube->AddComponent<MeshRenderer>()->SetMaterial(cubeMaterial);
	////cube->AddComponent<PlayerMovement>();
	//cube->GetTransform()->SetPosition({ 3.0f, 0.f ,0.f });
#pragma endregion
#pragma region light
	//const auto light = CreateLight();
	//light->data->type = LightType::Directional;
	//light->data->direction = glm::vec4(0.0f, -1.0f, 0.0f , 1.f);
	const auto dirlight = CreateLight();
	dirlight->data->direction = { 0.f, -sqrt(2), sqrt(2), 0.f };

	//const auto light2 = CreateLight();
	//light2->data->type = LightType::Point;
	//light2->data->direction = glm::vec4(0.0f, -1.0f, 0.0f , 1.f);
	//light2->GetGameObject()->GetTransform()->SetPosition(glm::vec3(6.0f, 4.0f, 20.0f));
	//light2->data->cos_cutoff = glm::radians(45.0f);
	//light2->data->cos_inner = glm::radians(30.0f);
	//light2->data->constant = 0.1f;
	//light2->data->linear = 0.01f;
	//light2->data->quadratic = 0.001f;
#pragma endregion
#pragma region rock
	Application::GetInstance()->UpdateLoadingScene("Creating Rock");
	//Rock
	rockMesh_ = MeshBuilder::GenerateOBJMTL("rock", "Assets/Low_Poly_Rock_001.obj", "Assets/Low_Poly_Rock_001.mtl");
    obj = CreateGameObject();
	obj->AddComponent<MeshRenderer>();
	obj->AddComponent<MeshFilter>()->SetMesh(rockMesh_);
	obj->AddComponent<RigidBody>()->SetMass(15.0f);
	obj->GetTransform()->SetScale(glm::vec3(5, 5, 5));
	obj->GetTransform()->SetPosition(glm::vec3(0, 0, 80));

	auto rockCol = obj->AddComponent<SphereCollider>();
	rockCol->SetRadius(2.0f);
	rockCol->isTrigger = true;
	rockCol->Initialize();
	obj->AddComponent<ThrowableRock>();
#pragma endregion
#pragma region carnival stuff
#pragma region stall
	//Stall
	Application::GetInstance()->UpdateLoadingScene("Creating Stall.");
	//stallMesh_ = MeshBuilder::GenerateOBJMTL("Stall", "Assets/Stall/Obj/Stall.obj", "Assets/Stall/Obj/Stall.mtl");
	//if (stallMesh_)
	//{
	//	stallMesh_->materials[2]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Stall/Img/redStall.png");
	//	//stallMesh_->materials[1]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Stall/Img/wood.jpg");
	//}
	//for (auto& i : stallMesh_->materials)
	//	mats_.push_back(i);

	std::shared_ptr<GameObject> stall = CreateGameObject();
	//stall->AddComponent<MeshRenderer>();
	//stall->AddComponent<MeshFilter>()->SetMesh(stallMesh_);
	stall->AddComponent<ModelRenderer>()->LoadModel("Assets/Stall/Obj/RedStall.obj");
	stall->GetTransform()->SetPosition(glm::vec3(20.f, -20.f, 110.f));
	stall->GetTransform()->SetScale(glm::vec3(3.0f, 3.0f, 3.0f));
	stall->GetTransform()->SetEulerAngles(glm::vec3(0.f, 180.f, 0.f));
	//stall->AddComponent<RigidBody>()->SetMass(5.0f);


	auto stallCol = stall->AddComponent<BoxCollider>();
    stallCol->SetOffset(glm::vec3(0.f, 5.f, -18.f));
	stallCol->SetSize(glm::vec3(35.f, 15.f, 2.f));
	stallCol->isTrigger = true;
	stallCol->Initialize();

	std::shared_ptr<GameObject> stallcol2_= CreateGameObject();


	auto stallCol2 = stallcol2_->AddComponent<BoxCollider>();
	stallcol2_->GetTransform()->SetPosition(stall->GetTransform()->GetPosition());
	stallCol2->SetOffset(glm::vec3(-18.f, 5.f, 0.f));
	stallCol2->SetSize(glm::vec3(2.f, 15.f, 32.f));
	stallCol2->isTrigger = true;
	stallCol2->Initialize();

	std::shared_ptr<GameObject> stallcol3_ = CreateGameObject();
	auto stallCol3 = stallcol3_->AddComponent<BoxCollider>();
	stallcol3_->GetTransform()->SetPosition(stall->GetTransform()->GetPosition());
	stallCol3->SetOffset(glm::vec3(18.f, 5.f, 0.f));
	stallCol3->SetSize(glm::vec3(2.f, 15.f, 32.f));
	stallCol3->isTrigger = true;
	stallCol3->Initialize();

	std::shared_ptr<GameObject> stallcolfloor_ = CreateGameObject();
	auto stallColfloor = stallcolfloor_->AddComponent<BoxCollider>();
	stallcolfloor_->AddComponent<StallFloor>();
	stallcolfloor_->GetTransform()->SetPosition(stall->GetTransform()->GetPosition());
	stallColfloor->SetOffset(glm::vec3(3.f, 0.f, 4.f));
	stallColfloor->SetSize(glm::vec3(40.f, 10.f, 40.f));
	stallColfloor->isTrigger = true;
	stallColfloor->Initialize();
	//stallcolfloor_->AddComponent<ThrowableRock>();
#pragma endregion
#pragma region stool
	Application::GetInstance()->UpdateLoadingScene("Creating Stool.");
	//Stool
	stoolMesh_ = MeshBuilder::GenerateOBJMTL("stool", "Assets/Stool/Obj/frehgstol.obj", "Assets/Stool/Obj/frehgstol.mtl");
	//if (stoolMesh_)
	//{
	//	stoolMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Stool/Img/Stool_Diffuse.tga");
	//	stoolMesh_->materials[1]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Stool/Img/Stool_Normal.tga");
	//	stoolMesh_->materials[2]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Stool/Img/Stool_Specular.tga");ds
	//}
	mats_.push_back(stoolMesh_->materials.front());

	std::shared_ptr<GameObject> stool = CreateGameObject();
	stool->AddComponent<MeshRenderer>();
	stool->AddComponent<MeshFilter>()->SetMesh(stoolMesh_);
	stool->GetTransform()->SetPosition(glm::vec3(20.f, -10.f,100.f));
	stool->GetTransform()->SetScale(glm::vec3(3.0f,3.0f,3.0f));
	//stool->AddComponent<RigidBody>()->SetMass(20.0f);
	
	auto stoolCol = stool->AddComponent<BoxCollider>();
	stoolCol->SetOffset(glm::vec3(-2.7f, -2.f, 7.f));
	stoolCol->SetSize(glm::vec3(7.3f, 18.f, 6.5f));
	stoolCol->isTrigger = true;
	stoolCol->Initialize();

	//stool->AddComponent<ThrowableRock>();
#pragma endregion
#pragma region bottle
	Application::GetInstance()->UpdateLoadingScene("Creating Bottles.");
	std::vector<glm::vec3> bottlepositions
	{
		{ 17, 2.f, 106 },
		{ 18, -3.f, 106 },
		{ 16, -3.f, 106 },
	};

	std::vector<std::string> bottletags
	{
		"bottle1",
		"bottle2",
		"bottle3",
	};
	//Bottle
	bottleMesh_ = MeshBuilder::GenerateOBJMTL("bottle", "Assets/Bottle/Obj/Bottle.obj", "Assets/Bottle/Obj/Bottle.mtl");
	if (bottleMesh_)
	{
		bottleMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Bottle/Img/BOTTLE_Base_color.png");
		bottleMesh_->materials[1]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Bottle/Img/BOTTLE_Normal_OpenGL.png");
	}
	for (auto& i : bottleMesh_->materials)
		mats_.push_back(i);
	for (int i = 0; i < bottlepositions.size(); i++)
	{
		std::shared_ptr<GameObject> bottle = CreateGameObject();
		bottle->AddComponent<MeshRenderer>();
		bottle->AddComponent<MeshFilter>()->SetMesh(bottleMesh_);
		bottle->GetTransform()->SetPosition(bottlepositions[i]);
		bottle->GetTransform()->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		bottle->GetTransform()->SetEulerAngles(glm::vec3(0.f, 0.f, 0.f));
		bottle->tag = bottletags[i];
		bottle->AddComponent<RigidBody>()->SetMass(100.0f);

		auto bottleCol = bottle->AddComponent<BoxCollider>();
		bottleCol->SetOffset(glm::vec3(0.f, 2.8f, 0.f));
		bottleCol->SetSize(glm::vec3(2.f, 5.5f, 2.f));
		bottleCol->isTrigger = true;
		bottleCol->Initialize();
		//bottle->AddComponent<ThrowableRock>();
	}

	std::vector<glm::vec3> wallpositions
	{
		{ 60, 0, 0 },
		{ -10, 0, 0 },
		{ 0, 0, 120},
		{ 0, 0, -10}
	};

	std::vector<glm::vec3> wallscale
	{
		{5, 500, 500},
		{5, 500, 500},
		{500, 500, 5},
		{500, 500, 5}
	};

	for (int i = 0; i < wallpositions.size(); i++)
	{
		std::shared_ptr<GameObject> wall = CreateGameObject();
		//wall->AddComponent<RigidBody>()->SetMass(200.0f);
		wall->GetTransform()->SetPosition(wallpositions[i]);
		auto wallCol = wall->AddComponent<BoxCollider>();
		//wallCol->SetPosition(wallpositions[i]);
	    wallCol->SetSize(wallscale[i]);
		wallCol->isTrigger = true;
		wallCol->Initialize();
	}
#pragma endregion
#pragma endregion carnival stuff

#pragma region envrionment stuff
#pragma region fern
	Application::GetInstance()->UpdateLoadingScene("Creating Extra details.");
	fernMesh_ = MeshBuilder::GenerateOBJ("Fern", "Assets/fern.obj");

	Material* fernMaterial = new Material();
	fernMaterial->data->_ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	fernMaterial->data->_diffuse = { 0.05f * 4.f, 0.09f * 4.f, 0.05f * 4.f, 1.0f };
	fernMaterial->materialShader = ShaderLibrary::GetShaderFromIllumModel(1);
	fernMaterial->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/fern.png");
	mats_.push_back(fernMaterial);
#pragma endregion
#pragma region treemeshes
	birchTreeMesh_ = MeshBuilder::GenerateOBJMTL("tree", "Assets/tree-bake-upload/source/tree_bake_upload.obj", "Assets/tree-bake-upload/source/tree_bake_upload.mtl");

	//auto treeMeshTexture = Texture::LoadTexture("Assets/export/trans.png");

	if (birchTreeMesh_)
	{
		birchTreeMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/tree-bake-upload/textures/birch.png");
		birchTreeMesh_->materials[1]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/tree-bake-upload/textures/BarkBaked.png");
	}
	for (auto& i : birchTreeMesh_->materials)
		mats_.push_back(i);


	treeMesh_ = MeshBuilder::GenerateOBJMTL("tree2", "Assets/Tree 02/Tree.obj", "Assets/Tree 02/Tree.mtl");

	if (treeMesh_)
	{
		treeMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Tree 02/bark_0004.jpg");
		treeMesh_->materials[1]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Tree 02/DB2X2_L01.png");
	}

	for (auto& i : treeMesh_->materials)
		mats_.push_back(i);
	//if (treeMesh)
	//for (auto& i : treeMesh->materials)
	//{
	//	i->textureMaps[Material::DIFFUSE] = treeMeshTexture;
	//}
#pragma endregion
#pragma region birch_trees
	Application::GetInstance()->UpdateLoadingScene("Creating trees.");
	std::vector<glm::vec3> birch_tree_positions
	{
		{ 10, -15.75f, 10 },
		{ 40, -15.75f, 10 },

		{ -30, -12.75f, -60 },
		{ -40, -10.75f, 80 },

		{ 200, -11.75f, -4 },
		{ -200, -11.75f, 150 },

		{ -300, -11.75f, -50 },
		{ 300, -12.75f, -60 },

		{ -50, -10.75f, -150 },
		{ 90, -13.75f, -320 },

		{ -200, -11.75f, 150 },
		{ 320, -12.75f, 320 },
	};

	std::vector<glm::vec3> birch_tree_scales
	{
		{ 4, 4, 4 },
		{ 3.5f, 3, 3.5f },
		{ 4, 4, 4 },
		{ 3.5f, 3, 3.5f },
		{ 4, 4, 4 },
		{ 5, 5, 5 },
		{ 3.5f, 3, 3.5f },
		{ 3.5f, 5, 3.5f },
		{ 4, 4, 4 },
		{ 4, 4, 4 },
		{ 5, 5, 5 },
		{ 5, 5, 5 },
	};

	std::vector<glm::vec3> birch_tree_rotations
	{
		{},
		{ 0, 180, 0 },
		{},
		{},
		{ 0, 90, 0 },
		{ 0, -90, 0 },
		{},
		{ 0, 180, 0 },
		{ 0, 90, 0 },
		{ 0, 90, 0 },
		{},
		{ 0, 180, 0 },
	};

	for (int i = 0; i < birch_tree_positions.size(); ++i)
	{
		std::shared_ptr<GameObject> tree_obj = CreateGameObject();
		tree_obj->AddComponent<MeshRenderer>()->renderOrder = 1;
		tree_obj->AddComponent<MeshFilter>()->SetMesh(birchTreeMesh_);
		tree_obj->GetTransform()->SetPosition(birch_tree_positions[i]);
		tree_obj->GetTransform()->SetScale(birch_tree_scales[i]);
		tree_obj->GetTransform()->SetEulerAngles(birch_tree_rotations[i]);
	}
#pragma endregion
#pragma region trees
	std::vector<glm::vec3> tree_positions
	{
		{ 20, -16.75f, 60 },
		{ 200, 0.75f, 300 },
		{ -160, -15.75f, 85 },
		{ 200, -12.75f, 10 },
		{ 380, -11.75f, 10 },
		{ -200, -5.75f, 300 },
	};

	std::vector<glm::vec3> tree_scales
	{
		{ 4, 4, 4 },
		{ 4, 5, 4 },
		{ 4, 4, 4 },
		{ 4, 4, 4 },
		{ 4, 5, 4 },
		{ 4, 4, 4 },
	};

	std::vector<glm::vec3> tree_rotations
	{
		{},
		{},
		{},
		{},
		{ 0, 90, 0},
		{ 0, 180, 0 },
	};


	for (int i = 0; i < tree_positions.size(); ++i)
	{
		std::shared_ptr<GameObject> tree_obj = CreateGameObject();
		tree_obj->AddComponent<MeshRenderer>()->renderOrder = 1;
		tree_obj->AddComponent<MeshFilter>()->SetMesh(treeMesh_);
		tree_obj->GetTransform()->SetPosition(tree_positions[i]);
		tree_obj->GetTransform()->SetScale(tree_scales[i]);
		tree_obj->GetTransform()->SetEulerAngles(tree_rotations[i]);
	}
#pragma endregion
#pragma region ferns
	Application::GetInstance()->UpdateLoadingScene("Creating ferns.");
	std::vector<glm::vec3> fern_positions
	{
		{ 100, -10.0f, 50 },
		{ 200, -14.5f, 150 },
		{ -120, -13.2f, -50 },
		{ 10, -7.5f, -300 },
	};

	std::vector<glm::vec3> fern_scales
	{
		{ 1, 1, 1 },
		{ 1, 1, 1 },
		{ 1, 1, 1 },
		{ 1, 1, 1 },
	};

	std::vector<glm::vec3> fern_rotations
	{
		{ 20, 5, 0 },
		{ 0, 90, 0 },
		{},
		{ -15, 180, 0 },
	};


	for (int i = 0; i < fern_positions.size(); ++i)
	{
		std::shared_ptr<GameObject> fern = CreateGameObject();
		auto mr = fern->AddComponent<MeshRenderer>();
		mr->renderOrder = 1;
		mr->SetMaterial(fernMaterial);
		fern->AddComponent<MeshFilter>()->SetMesh(fernMesh_);
		fern->GetTransform()->SetPosition(fern_positions[i]);
		fern->GetTransform()->SetScale(fern_scales[i]);
		fern->GetTransform()->SetEulerAngles(fern_rotations[i]);
	}
#pragma endregion
#pragma region flowers
#pragma endregion
#pragma region fountain
	//fountainMesh_ = MeshBuilder::GenerateOBJMTL("Fountain", "Assets/fountain/fountain.obj", "Assets/fountain/fountain.mtl");

	//if (fountainMesh_)
	//{
	//	fountainMesh_->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/fountain/granite.jpg");
	//}

	//std::shared_ptr<GameObject> fountain = CreateGameObject();
	//fountain->AddComponent<MeshRenderer>();
	//fountain->AddComponent<MeshFilter>()->SetMesh(fountainMesh_);
	//fountain->GetTransform()->SetScale(glm::vec3(7.5f));
	//fountain->GetTransform()->SetPosition({ 0, -14, 150 });
	//fountain->GetTransform()->SetEulerAngles({ -4, 0, -2.5f });

	//std::shared_ptr<GameObject> fountainText = CreateGameObject();
	//auto ftComponent = fountainText->AddComponent<Text>();
	//ftComponent->SetText("Try hitting the rocks with a pebble");
	//ftComponent->onScreen = false;
	//fountainText->GetTransform()->SetScale(glm::vec3(1, 1, 1));
	//fountainText->GetTransform()->SetPosition({ -7, -13, 147 });
	////fountainText->GetTransform()->SetPosition({ 0, 0, 0 });
	//fountainText->GetTransform()->SetEulerAngles({ 0, 10, 0 });
	//ftComponent->font = inconsolataFont_;
	//ftComponent->colour = { 0.f, 1.0f, 0.f };
#pragma endregion
#pragma endregion environment stuff

#pragma region other stuff
#pragma region ahri
	//ahriMesh_ = MeshBuilder::GenerateOBJMTL("Girl", "Assets/Ahri/source/Ahri/Ahri.obj", "Assets/Ahri/source/Ahri/Ahri.mtl");

	//if (ahriMesh_)
	//{
	//	ahriMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Ahri/source/Ahri/Tex_0002_0.png");
	//}

	inconsolataFont_ = Font::LoadFont("Assets/fonts/inconsolata.tga", "Assets/fonts/inconsolata.csv");
	inconsolataFont_->fontMultiplier = 0.014f;

	//std::shared_ptr<GameObject> ahri = CreateGameObject();
	//ahri->AddComponent<MeshRenderer>();
	//ahri->AddComponent<MeshFilter>()->SetMesh(ahriMesh_);
	//
	//ahri->GetTransform()->SetPosition({ 65.0f, -15.8f , 100.0f });
	//ahri->GetTransform()->SetEulerAngles({ 0, -67, 0 });
	//ahri->GetTransform()->SetScale({ 6,6,6 });

	//auto ahriTalkable = ahri->AddComponent<Ahri>();
	//auto ahriTalkable = ahri->AddComponent<Talkable>();
	//ahriTalkable->BindDialogue(
	//	{
	//		"Hey human!",
	//		"Can you find my precious toy for me?",
	//		"I will reward you with a hefty reward!"
	//	}
	//);
#pragma endregion
#pragma region crosshair

	Material* crosshairMaterial = new Material(
		{ 1.0f, 0.f, 1.0f, 1.0f },
		shaderLib_->Get("Illum 0")
	);
	mats_.push_back(crosshairMaterial);

	crosshairMesh_ = MeshBuilder::GenerateQuad("Quad", glm::vec4(1.0f));
	auto crosshair = CreateGameObject();
	crosshair->AddComponent<UIRenderer>()->SetMaterial(crosshairMaterial);
	crosshair->GetComponent<UIRenderer>()->renderOrder = 2;
	crosshair->AddComponent<MeshFilter>()->SetMesh(crosshairMesh_);
	crosshair->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() * 0.5f, Application::GetInstance()->GetWindowHeight() * 0.5f, 0});
	crosshair->GetTransform()->SetScale({ 2, 2, 1 });
		//textureCube->texture = new Texture("Assets/poppychibi.tga");
#pragma endregion
#pragma region question mark
	//Material* textureCube = new Material(
	//	{ 0.6f, 0.6f, 0.6f, 1.0f },
	//	{ 0.3f, 0.3f, 0.3f, 1.0f },
	//	{ 0.5f, 0.5f, 0.5f, 1.0f },
	//	95.f,
	//	Shader::DefaultShader
	//);

	//textureCube->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/question_mark.png");

	//std::shared_ptr<GameObject> question_mark = CreateGameObject();
	//auto tc_renderer = question_mark->AddComponent<UIRenderer>();
	//tc_renderer->SetMaterial(textureCube);
	//tc_renderer->billboarded = true;

	//question_mark->AddComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMesh(SceneRenderer::GEO_QUAD)));
	//question_mark->GetTransform()->SetPosition({ 65.0f, -2.f, 100.f });
	//question_mark->GetTransform()->SetEulerAngles({ -90.f, 0.f, 0.f });
	//question_mark->GetTransform()->SetScale({ 2.5f, 2.5f, 1.f });
	//auto qc = question_mark->AddComponent<SphereCollider>();
	//qc->Initialize();
	//auto ib = question_mark->AddComponent<InteractButton>();
	//question_mark->tag = "Interact";
	//ib->Bind(ahriTalkable);
#pragma endregion
#pragma endregion
#pragma region skybox
	skybox_ = new Skybox({
			"Assets/skybox/posx.tga",
			"Assets/skybox/negx.tga",
			"Assets/skybox/posy.tga",
			"Assets/skybox/negy.tga",
			"Assets/skybox/posz.tga",
			"Assets/skybox/negz.tga"
		});
#pragma endregion
#pragma region fpscounter
	std::shared_ptr<GameObject> text = CreateGameObject();
	fpsCounter_ = text->AddComponent<Text>();
	fpsCounter_->SetText("FPS: ");
	text->GetTransform()->SetPosition({ 20, Application::GetInstance()->GetWindowHeight() - 40, 20 });
	//text->GetTransform()->SetPosition({ 0, 1240, 0 });
	text->GetTransform()->SetScale({ 40, 45, 1 });
	text->GetTransform()->SetEulerAngles({ 0, 0, 0 });
	fpsCounter_->font = inconsolataFont_;

	//text->AddComponent<PlayerMovement>();
	fpsCounter_->onScreen = true;	
#pragma endregion
#pragma region terrain
	auto terrain = Terrain::GenerateTerrain(
		"Assets/heightmap.png",
		"Assets/grass.png",
		"Assets/blendMap.png",
		"Assets/mud.png",
		"Assets/grassFlowers.png",
		"Assets/path.png"
	);

	if (terrain)
	{
		objects_.push_back(terrain);

		auto& terrain_mat = objects_.back()->GetComponent<MeshFilter>()->GetMesh()->materials.back();
		terrain_mat->tilingFactor = 40.f;

		auto& terrain_mat_data = terrain_mat->data;

		//terrain_mat_data->_diffuse = terrain_mat_data->_colour;
		terrain_mat_data->_specular = { 0.07f, 0.4f, 0.07f, 1.0f };
		terrain_mat_data->_ambient = { 0.15f, 0.15f, 0.15f, 1.0f };
		terrain_mat_data->_shininess = 255.0f;
		terrain_mat_data->_diffuse = {86.f / 255.f, 125.f / 255.f, 70.f / 255.f, 1.0};

		terrain->GetTransform()->SetPosition({ -400, -60, -400 });
		//terrain->Update(0.01);
		terrain->GetComponent<TerrainCollider>()->Initialize();
	}
#pragma endregion
#pragma region lantern
	//lanternMesh_ = MeshBuilder::GenerateOBJMTL("Lantern", "Assets/lantern/lantern.obj", "Assets/lantern/lantern.mtl");

	//std::vector<glm::vec3> lantern_positions
	//{
	//	{20,  terrain->GetHeight(20, 20), 20},
	//	{240, terrain->GetHeight(240, 240) + 0.5f, 240 },
	//	{-50, terrain->GetHeight(-50, 100) + 0.5f, 100 },
	//	{-59, terrain->GetHeight(-59, -40) + 0.5f, -40 },
	//};

	//std::vector<glm::vec3> lantern_scales
	//{
	//	{ 4, 4, 4},
	//	{ 4, 4, 4},
	//	{ 4, 4, 4},
	//	{ 4, 4, 4},
	//};

	//std::vector<glm::vec3> lantern_rotations
	//{
	//	{},
	//	{0, 90, 0},
	//	{},
	//	{},
	//};

	//std::vector<glm::vec4> lantern_light_colours
	//{
	//	{ 0.8f, 0, 0, 1.0f},
	//	{ 0.0f, 0.8f, 0.2f, 1.0f},
	//	{ 0.8f, 0.0f, 0.8f, 1.0f},
	//	{ 0.f, 0.8f, 0.f, 1.0f},
	//};

	//for (int i = 0; i < lantern_positions.size(); ++i)
	//{
	//	auto lanternLight = CreateLight();

	//	lanternLight->data->type = LightType::Point;
	//	lanternLight->data->cos_cutoff = glm::radians(45.0f);
	//	lanternLight->data->cos_inner = glm::radians(30.0f);
	//	lanternLight->data->constant = 0.1f;
	//	lanternLight->data->linear = 0.01f;
	//	lanternLight->data->quadratic = 0.001f;

	//	lanternLight->data->colour = lantern_light_colours[i];

	//	auto lanternObject = lanternLight->GetGameObject();
	//	auto mr = lanternObject->AddComponent<MeshRenderer>();
	//	mr->renderOrder = 0;
	//	lanternObject->AddComponent<MeshFilter>()->SetMesh(lanternMesh_);
	//	lanternObject->GetTransform()->SetPosition(lantern_positions[i]);
	//	lanternObject->GetTransform()->SetScale(lantern_scales[i]);
	//	lanternObject->GetTransform()->SetEulerAngles(lantern_rotations[i]);
	//	
	//	auto col = lanternObject->AddComponent<SphereCollider>();
	//	col->SetRadius(0.5f);
	//	col->Update(0);

	//	auto rb = lanternObject->AddComponent<RigidBody>();
	//	rb->SetMass(3.0f);
	//	rb->Update(0);

	//	lanternObject->AddComponent<ThrowableRock>();
	//}
#pragma endregion
#pragma region reward
	//Material* envMapMat = new Material();
	//envMapMat->materialShader = shaderLib_->Get("Environment Map");

	//auto reward = CreateGameObject();
	//reward->AddComponent<MeshRenderer>()->SetMaterial(envMapMat);
	//reward->AddComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMesh(SceneRenderer::GEO_BALL)));
	//reward->GetTransform()->SetScale(glm::vec3(3.f));
	//reward->AddComponent<SphereCollider>();
	//auto rewardrb = reward->AddComponent<RigidBody>();
	//rewardrb->SetMass(1.f);
	//rewardrb->SetRestitution(0.9f);

	//reward->SetActive(false);
#pragma endregion
#pragma region playerbase
	Application::GetInstance()->UpdateLoadingScene("Setting up the player.");
	//std::shared_ptr<GameObject> environmentMappedSphere = CreateGameObject();
	//environmentMappedSphere->AddComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMesh(SceneRenderer::GEO_BALL)));
	//environmentMappedSphere->AddComponent<MeshRenderer>()->SetMaterial(envMapMat);
	//environmentMappedSphere->GetTransform()->SetPosition({ 0, 10, 0 });
	//environmentMappedSphere->AddComponent<RigidBody>();
	//environmentMappedSphere->AddComponent<SphereCollider>();
	//environmentMappedSphere->AddComponent<PlayerMovement>();
	mouseIcon_ = MeshBuilder::GenerateQuad("Mouse Icon", glm::vec4(1.0f));
	mats_.push_back(new Material());
	mats_.back()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/mouse.png");
	mats_.back()->materialShader = ShaderLibrary::GetShaderFromIllumModel(0);
	mouseIcon_->materials.push_back(mats_.back());

	auto mouse_down = CreateGameObject();
	mouse_down->AddComponent<MeshFilter>()->SetMesh(mouseIcon_);
	mouse_down->AddComponent<UIRenderer>()->billboarded = true;
	mouse_down->GetComponent<UIRenderer>()->renderOrder = 3;
	mouse_down->GetTransform()->SetScale({ 4, 4, 1 });
	mouse_down->SetActive(false);


	playerBase_ = CreateGameObject();
	//playerBase->AddComponent<MeshFilter>()->SetMesh(playerModel[TORSO]);
	//playerBase->AddComponent<MeshRenderer>();
	playerBase_->GetTransform()->SetPosition({ 0, -10, 60 });

	playerBase_->GetTransform()->SetScale({ 1, 1, 1 });
	//environmentMappedSphere->AddComponent<RigidBody>();
	//player->AddComponent<SphereCollider>();
	auto pc = playerBase_->AddComponent<PlayerController>();
	pc->SetTerrain(terrain);
	pc->BindCamera(cameras_[0]);
	pc->BindCrosshair(crosshair);
	pc->BindIcon(mouse_down.get());

	auto instruction = CreateGameObject();
	auto textInstruction = instruction->AddComponent<Text>();
	textInstruction->SetText("Hold X to increase power, Hold Z to decrease power.");
	textInstruction->font = inconsolataFont_;
	textInstruction->onScreen = true;
	textInstruction->colour = { 0.0, 1, 0 };
	instruction->GetTransform()->SetScale(glm::vec3(40, 45, 1));

#pragma endregion
#pragma region PLAYER_MODEL
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
#pragma endregion PLAYER_MODEL
#pragma region cameracontroller
	camera_controller->Follow(playerBase_->GetTransform());
	//pc->CameraFollowTransform(playerHead->GetTransform());
	pc->SetCameraPositionOffset({ 0, 9.5f, 0 });
#pragma endregion
#pragma region flashlight
	//flashlightMesh_ = MeshBuilder::GenerateOBJMTL("Flashlight", "Assets/fl.obj", "Assets/fl.mtl");
	//flashlightMesh_->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/fl.jpg");

	//auto flashlightLight = CreateLight();
	//flashlightLight->data->type = LightType::Spot;
	//flashlightLight->data->cos_cutoff = glm::radians(45.0f);
	//flashlightLight->data->cos_inner = glm::radians(30.0f);
	//flashlightLight->data->constant = 0.1f;
	//flashlightLight->data->linear = 0.01f;
	//flashlightLight->data->quadratic = 0.001f;

	//auto flashlight = flashlightLight->GetGameObject();
	//flashlight->GetTransform()->SetPosition({100, -10, 50});
	//flashlight->AddComponent<MeshRenderer>();
	//flashlight->AddComponent<MeshFilter>()->SetMesh(flashlightMesh_);
	//flashlight->GetTransform()->SetScale(glm::vec3(0.005f));
	//auto sc = flashlight->AddComponent<SpotlightController>();
	//sc->BindRay(pc->GetRay());

	//flashlight->AddComponent<ThrowableRock>();
	//auto flashlightCol = flashlight->AddComponent<SphereCollider>();
	//auto flashlightRb = flashlight->AddComponent<RigidBody>();
	//flashlightRb->SetMass(8.f);
	//flashlightRb->SetRestitution(0.2f);
	//flashlightCol->Initialize();
	//flashlightCol->SetRadius(0.4f);
#pragma endregion
#pragma region rockpile
	Application::GetInstance()->UpdateLoadingScene("Setting up rock piles.");
	rockPileMesh_ = MeshBuilder::GenerateOBJMTL("Rock pile", "Assets/rockpile/rocks.obj", "Assets/rockpile/rocks.mtl");

	if (rockPileMesh_)
	{
		rockPileMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/001.png");
		rockPileMesh_->materials[1]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/001.png");
		rockPileMesh_->materials[2]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/003.png");
		rockPileMesh_->materials[3]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/003.png");
		rockPileMesh_->materials[4]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/002.png");
	}
	mats_.push_back(rockPileMesh_->materials[0]);
	mats_.push_back(rockPileMesh_->materials[2]);
	mats_.push_back(rockPileMesh_->materials[4]);

	std::vector<glm::vec3> rockPilePositions
	{
		{ -40, 0, 50 },
		{ 300, 0, -143 },
		{ 200, 0, -300 },
		{ -100, 0, 68 },
		{ -200, 0, 95 },
		{ -40, 0, -40 },
		{ -120, 0, 305 },
		{ 1, 0, 377 }
	};

	if (terrain)
	{
		for (int i = 0; i < rockPilePositions.size(); ++i)
		{
			auto rockPile = CreateGameObject();
			rockPile->AddComponent<MeshRenderer>();
			auto cr = rockPile->AddComponent<CoveringRock>();

			rockPile->AddComponent<MeshFilter>()->SetMesh(rockPileMesh_);
			rockPile->GetTransform()->SetScale(glm::vec3(12.f));
			rockPile->GetTransform()->SetPosition({ rockPilePositions[i].x, terrain->GetHeight(rockPilePositions[i].x, rockPilePositions[i].z), rockPilePositions[i].z });
			auto col = rockPile->AddComponent<SphereCollider>();
			col->SetRadius(0.6f);
			col->Initialize();
		}
	}

	portal = MeshBuilder::GenerateQuad("Arcade Portal", glm::vec4(1.0f));
	mats_.push_back(new Material());
	mats_.back()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/MainScene/portal.png");
	portal->materials.push_back(mats_.back());

	auto PortalLight = CreateLight();
	PortalLight->data->type = LightType::Point;
	PortalLight->data->cos_cutoff = glm::radians(45.0f);
	PortalLight->data->cos_inner = glm::radians(30.0f);
	PortalLight->data->constant = 0.1f;
	PortalLight->data->linear = 0.01f;
	PortalLight->data->quadratic = 0.001f;
	PortalLight->data->colour = { 102.f/255.f, 153.f/255.f, 204.f/255.f, 1.0f };
	PortalLight->data->power = 1.0f;

	GameObject* ScenePortal = PortalLight->GetGameObject();

	ScenePortal->AddComponent<UIRenderer>()->billboarded = true;
	ScenePortal->AddComponent<MeshFilter>()->SetMesh(portal);
	ScenePortal->GetTransform()->SetPosition({ 25, -8, -6});
	ScenePortal->GetTransform()->SetScale({ 10, 15, 1 });
	ScenePortal->AddComponent<Portal>()->sceneName = "MainScene";

#pragma endregion
	//Update(0.01);
	sceneCamera_ = cameras_[0];
#pragma endregion other stuff
	sceneTime = 0;
}

void BottleThrowScene::Update(double dt)
{
	fpsCounter_->SetText("FPS: " + std::to_string(static_cast<int>(1.0 / dt)));
	if (Input::GetInstance()->IsKeyPressed('N') && !isChangingCamera_)
	{
		//static bool player = true;
		static int index = 1;
		SwitchCamera(cameras_[index]);
		++index;
		if (index == 2)
			index = 0;
	}

	if (glm::distance(obj->GetTransform()->GetWorldPosition(), playerBase_->GetTransform()->GetWorldPosition()) > 50)
	{
		sceneTime += dt;
		if (int(sceneTime) % 6 == 0)
		{
			obj->GetTransform()->SetPosition(glm::vec3(20, -15, 80));
			obj->GetComponent<RigidBody>()->SetVelocity(glm::vec3(0));
			sceneTime = 1;
		}
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
		{
			return;
		}
	}


	CameraInterp();

	static Input* input = Input::GetInstance();
	input->Update();

}

void BottleThrowScene::Render()
{
	if (isChangingScene_)
	{
		Application::GetInstance()->LoadScene(sceneNameToChangeTo_);
		isChangingScene_ = false;
		return;
	}
#if _DEBUG
	if (Input::GetInstance()->IsKeyDown('B'))
	{
		Application::GetInstance()->LoadScene("MainScene");
		return;
	}
#endif
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
				shaderLib_, skybox_, lights_, lights_.size()
			);
	}


	for (const auto& i : objects_)
	{
		if (i->ActiveSelf())
			i->Render();
	}

	renderer_->EndScene();
}

void BottleThrowScene::Exit()
{
	//for (auto& i : objects_)
	//	if (i)
	//	{
	//		delete i;
	//		i = nullptr;
	//	}
	delete mouseIcon_;
	mouseIcon_ = nullptr;
	delete fountainMesh_;
	fountainMesh_ = nullptr;
	delete ahriMesh_;
	ahriMesh_ = nullptr;
	delete rockMesh_;
	rockMesh_ = nullptr;
	delete stallMesh_;
	stallMesh_ = nullptr;
	delete nanachiMesh_;
	nanachiMesh_ = nullptr;
	delete houseMesh_;
	houseMesh_ = nullptr;
	delete hutaoMesh_;
	hutaoMesh_ = nullptr;
	delete rockPileMesh_;
	rockPileMesh_ = nullptr;
	delete teddyBearMesh_;
	teddyBearMesh_ = nullptr;

	delete lanternMesh_;
	lanternMesh_ = nullptr;
	delete bottleMesh_;
	bottleMesh_ = nullptr;
	delete stoolMesh_;
	stoolMesh_ = nullptr;

	delete treeMesh_;
	treeMesh_ = nullptr;
	delete birchTreeMesh_;
	birchTreeMesh_ = nullptr;
	delete fernMesh_;
	fernMesh_ = nullptr;
	delete flowerPatchMesh_;
	flowerPatchMesh_ = nullptr;

	delete flashlightMesh_;
	flashlightMesh_ = nullptr;
	delete crosshairMesh_;
	crosshairMesh_ = nullptr;

	delete portal;
	portal = nullptr;

	if (skybox_)
	{
		delete skybox_;
		skybox_ = nullptr;
	}
	delete inconsolataFont_;
	inconsolataFont_ = nullptr;

	for (const auto& i : playerModel_)
		delete i;

	for (const auto& i : mats_)
		delete i;

	mats_.clear();
	lights_.clear();
	objects_.clear();
}

Light* BottleThrowScene::CreateLight()
{
	std::shared_ptr<GameObject> temp = CreateGameObject();
	auto temp_light = temp->AddComponent<Light>(); 	
	lights_.emplace_back(temp_light->data);
	return temp_light;
}

void BottleThrowScene::CameraInterp()
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

void BottleThrowScene::SwitchCamera(Camera* newCamera)
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

BottleThrowScene::~BottleThrowScene()
{
}
