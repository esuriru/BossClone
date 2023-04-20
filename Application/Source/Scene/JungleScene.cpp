#include "JungleScene.h"
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
#include "Components/PlayerMovement.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

JungleScene::JungleScene()
	: shaderLib_(new ShaderLibrary())
	, isChangingCamera_(false)
{
}

void JungleScene::Init()
{
	//shaderLib_->Add("Default", Shader::DefaultShader);
	//shaderLib_->Add("1", ShaderLibrary::GetShaderFromIllumModel(1));
	//shaderLib_->Load("Illum 0", "Shader/Illum 0shader.vertexshader", "Shader/Illum 0shader.fragmentshader");

	//Shader::MappedShader =
	//	shaderLib_->Load("Mapped Shader", "Shader/normalmaps.vertexshader", "Shader/normalmaps.fragmentshader");

	//Shader::MappedShader->SetFlags(ShaderFlag::NORMAL_MAPPED);

	shaderLib_ = ShaderLibrary::GetInstance();
	auto envShader = shaderLib_->Get("Environment Map");
	envShader->SetFlags(ShaderFlag::ENVIRONMENT_MAPPED);

	Shader::MultitexturedShader = shaderLib_->Get("Terrain Shader");
	if (Shader::MultitexturedShader)
		Shader::MultitexturedShader->SetFlags(ShaderFlag::MULTI_TEXTURING);

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


	Material* cubeMaterial = new Material(
		{ 0.3f, 0.3f, 0.3f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 0.5f, 1.0f },
		15.f,
		Shader::DefaultShader
	);

	Material* white = new Material(
		{ 1.0f, 1.f, 1.0f, 1.0f },
		shaderLib_->Get("Illum 0")
	);

	//static std::shared_ptr<GameObject> cube = CreateGameObject();
	//cube->AddComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEOMETRY_TYPE::GEO_BALL]));
	//cube->AddComponent<MeshRenderer>()->SetMaterial(cubeMaterial);
	////cube->AddComponent<PlayerMovement>();
	//cube->GetTransform()->SetPosition({ 3.0f, 0.f ,0.f });

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

	fernMesh_ = MeshBuilder::GenerateOBJ("Fern", "Assets/fern.obj");

	Material* fernMaterial = new Material();
	fernMaterial->data->_ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	fernMaterial->data->_diffuse = { 0.05f * 4.f, 0.09f * 4.f, 0.05f * 4.f, 1.0f };
	fernMaterial->materialShader = ShaderLibrary::GetShaderFromIllumModel(1);
	fernMaterial->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/fern.png");


	birchTreeMesh_ = MeshBuilder::GenerateOBJMTL("tree", "Assets/tree-bake-upload/source/tree_bake_upload.obj", "Assets/tree-bake-upload/source/tree_bake_upload.mtl");

	//auto treeMeshTexture = Texture::LoadTexture("Assets/export/trans.png");

	if (birchTreeMesh_)
	{
		birchTreeMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/tree-bake-upload/textures/birch.png");
		birchTreeMesh_->materials[1]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/tree-bake-upload/textures/BarkBaked.png");
	}

	treeMesh_ = MeshBuilder::GenerateOBJMTL("tree2", "Assets/Tree 02/Tree.obj", "Assets/Tree 02/Tree.mtl");

	if (treeMesh_)
	{
		treeMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Tree 02/bark_0004.jpg");
		treeMesh_->materials[1]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Tree 02/DB2X2_L01.png");
	}

	//if (treeMesh)
	//for (auto& i : treeMesh->materials)
	//{
	//	i->textureMaps[Material::DIFFUSE] = treeMeshTexture;
	//}


	rockMesh_ = MeshBuilder::GenerateOBJMTL("rock", "Assets/Low_Poly_Rock_001.obj", "Assets/Low_Poly_Rock_001.mtl");
	std::shared_ptr<GameObject> obj = CreateGameObject();
	obj->GetTransform()->SetPosition({ 15, 30, 15 });
	obj->AddComponent<MeshRenderer>();
	obj->AddComponent<MeshFilter>()->SetMesh(rockMesh_);
	obj->AddComponent<RigidBody>()->SetMass(5.0f);

	auto rockCol = obj->AddComponent<BoxCollider>();
	//rockCol->SetRadius(0.4f);
	rockCol->isTrigger = true;
	rockCol->Initialize();

	obj->AddComponent<ThrowableRock>();

	//nanachiMesh = MeshBuilder::GenerateOBJ("Nanachi", "Assets/Nanachi.obj");

	//for (auto& m : fenceMesh->materials)
	//	m->data->_colour = { 0.2f, 0.2f, 0.2f, 1.0f };

	//obj->AddComponent<PlayerMovement>();

	//for (auto& m : treeMesh->materials)
	//	m->data->_colour = { 0.2f, 0.2f, 0.2f, 1.0f };


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

	flowerPatchMesh_ = MeshBuilder::GenerateOBJMTL("Flower patch", "Assets/flowers/obj/WC_Babiana_Blue_Flower.obj", "Assets/flowers/obj/WC_Babiana_Blue_Flower.mtl");

	if (flowerPatchMesh_)
	{
		flowerPatchMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/flowers/textures/WC_Blue_Flower.tga");
	}

	std::vector<glm::vec3> flower_positions
	{
		{0, -10, 240},
	};

	std::vector<glm::vec3> flower_scales
	{
		{ 0.25f, 0.25f, 0.25f },
	};

	std::vector<glm::vec3> flower_rotations
	{
		{},
	};

	for (int i = 0; i < flower_positions.size(); ++i)
	{
		std::shared_ptr<GameObject> fern = CreateGameObject();
		auto mr = fern->AddComponent<MeshRenderer>();
		mr->renderOrder = 1;
		//mr->SetMaterial(fernMaterial);
		fern->AddComponent<MeshFilter>()->SetMesh(flowerPatchMesh_);
		fern->GetTransform()->SetPosition(flower_positions[i]);
		fern->GetTransform()->SetScale(flower_scales[i]);
		fern->GetTransform()->SetEulerAngles(flower_rotations[i]);
	}

	//ahriMesh_ = MeshBuilder::GenerateOBJMTL("Girl", "Assets/Ahri/source/Ahri/Ahri.obj", "Assets/Ahri/source/Ahri/Ahri.mtl");

	// if (ahriMesh_)
	// {
	// 	ahriMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Ahri/source/Ahri/Tex_0002_0.png");
	// }

	//inconsolataFont_ = Font::LoadFont("Assets/fonts/inconsolata.tga", "Assets/fonts/inconsolata.csv");
	//inconsolataFont_->fontMultiplier = 0.014f;

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

	// teddyBearMesh_ = MeshBuilder::GenerateOBJMTL("Teddy", "Assets/teddy/teddy.obj", "Assets/teddy/teddy.mtl");
	
	// if (teddyBearMesh_)
	// {
	// 	teddyBearMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/teddy/textures/sugarBear_txt.png");
	// }

	//auto teddyBear = CreateGameObject();
	//teddyBear->AddComponent<MeshRenderer>();
	//teddyBear->AddComponent<MeshFilter>()->SetMesh(teddyBearMesh_);
	//teddyBear->GetTransform()->SetScale(glm::vec3(10.0f));
	//auto bearCol = teddyBear->AddComponent<SphereCollider>();
	//bearCol->SetRadius(0.5f);
	//teddyBear->AddComponent<RigidBody>();
	//auto teddyInt =  teddyBear->AddComponent<ThrowableRock>();
	//teddyInt->label = "Teddy";

	fountainMesh_ = MeshBuilder::GenerateOBJMTL("Fountain", "Assets/fountain/fountain.obj", "Assets/fountain/fountain.mtl");

	if (fountainMesh_)
	{
		fountainMesh_->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/fountain/granite.jpg");
	}

	std::shared_ptr<GameObject> fountain = CreateGameObject();
	fountain->AddComponent<MeshRenderer>();
	fountain->AddComponent<MeshFilter>()->SetMesh(fountainMesh_);
	fountain->GetTransform()->SetScale(glm::vec3(7.5f));
	fountain->GetTransform()->SetPosition({ 0, -14, 150 });
	fountain->GetTransform()->SetEulerAngles({ -4, 0, -2.5f });

	std::shared_ptr<GameObject> fountainText = CreateGameObject();
	auto ftComponent = fountainText->AddComponent<Text>();
	ftComponent->SetText("Try hitting the rocks with a pebble");
	ftComponent->onScreen = false;
	fountainText->GetTransform()->SetScale(glm::vec3(1, 1, 1));
	fountainText->GetTransform()->SetPosition({ -7, -13, 147 });
	//fountainText->GetTransform()->SetPosition({ 0, 0, 0 });
	fountainText->GetTransform()->SetEulerAngles({ 0, 10, 0 });
	ftComponent->font = inconsolataFont_;
	ftComponent->colour = { 0.f, 1.0f, 0.f };
		
	Material* textureCube = new Material(
		{ 0.6f, 0.6f, 0.6f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 0.5f, 1.0f },
		95.f,
		Shader::DefaultShader
	);

	Material* crosshairMaterial = new Material(
		{ 1.0f, 0.f, 1.0f, 1.0f },
		shaderLib_->Get("Illum 0")
	);

	sphereMesh_ = MeshBuilder::GenerateSphere("Sphere", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, 32, 1.f);
	auto crosshair = CreateGameObject();
	crosshair->AddComponent<UIRenderer>()->SetMaterial(crosshairMaterial);
	crosshair->GetComponent<UIRenderer>()->renderOrder = 2;
	crosshair->AddComponent<MeshFilter>()->SetMesh(sphereMesh_);
	crosshair->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() * 0.5f, Application::GetInstance()->GetWindowHeight() * 0.5f, 0});
	crosshair->GetTransform()->SetScale({ 2, 2, 1 });
	//textureCube->texture = new Texture("Assets/poppychibi.tga");

	textureCube->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/question_mark.png");

	std::shared_ptr<GameObject> question_mark = CreateGameObject();
	auto tc_renderer = question_mark->AddComponent<UIRenderer>();
	tc_renderer->SetMaterial(textureCube);
	tc_renderer->billboarded = true;

	quadMesh_ = MeshBuilder::GenerateQuad("Question Mark", glm::vec4(1.0f));
	question_mark->AddComponent<MeshFilter>()->SetMesh(quadMesh_);
	question_mark->GetTransform()->SetPosition({ 65.0f, -2.f, 100.f });
	question_mark->GetTransform()->SetEulerAngles({ -90.f, 0.f, 0.f });
	question_mark->GetTransform()->SetScale({ 2.5f, 2.5f, 1.f });
	auto qc = question_mark->AddComponent<SphereCollider>();
	qc->Initialize();
	auto ib = question_mark->AddComponent<InteractButton>();
	question_mark->tag = "Interact";
	//ib->Bind(ahriTalkable);

	Material* dialogueBoxMaterial = new Material(
		{ 1.0f, 0.f, 1.0f, 1.0f },
		shaderLib_->Get("Illum 0")
	);


	quadMesh2_ = MeshBuilder::GenerateQuad("Dialogue Box", glm::vec4(1.0f));
	dialogueBoxMaterial->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/dialoguebox.png");
	//dialogueBoxMaterial->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/megumin.png");
	dialogueBox_ = CreateGameObject();
	dialogueBox_->tag = "Dialogue Box";
	dialogueBox_->AddComponent<UIRenderer>()->renderOrder = 2;
	dialogueBox_->GetComponent<UIRenderer>()->SetMaterial(dialogueBoxMaterial);
	//_dialogueBox->GetComponent<UIRenderer>()->SetMaterial(white);
	dialogueBox_->AddComponent<MeshFilter>()->SetMesh(quadMesh2_);
	dialogueBox_->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() *0.5f, 120, 0 });
	//_dialogueBox->GetTransform()->SetPosition({ 0, 0, 0 });
	dialogueBox_->GetTransform()->SetScale({ Application::GetInstance()->GetWindowWidth() * 0.8f, Application::GetInstance()->GetWindowHeight() * 0.25f, 0});
	//_dialogueBox->GetTransform()->SetScale({  50, 50, 1 });
	//_dialogueBox->GetTransform()->SetEulerAngles({ 0, -90, 0 });

	auto dialogueTextObject = CreateGameObject();
	dialogueText_ = dialogueTextObject->AddComponent<Text>();
	dialogueText_->renderOrder = 2;
	dialogueText_->BindDialogueBox(dialogueBox_);
	dialogueText_->EnableDialogueBox(false);
	dialogueText_->font = inconsolataFont_;
	dialogueText_->onScreen = true;
	dialogueTextObject->GetTransform()->SetScale({ 50, 60, 1 });
	dialogueTextObject->GetTransform()->SetPosition({ 185, 120, 0 });

#if 0

	houseMesh = MeshBuilder::GenerateOBJMTL("house", "Assets/house_type01.obj", "Assets/house_type01.mtl");

	std::shared_ptr<GameObject> house_obj= CreateGameObject();
	house_obj->AddComponent<MeshRenderer>();
	house_obj->AddComponent<MeshFilter>()->SetMesh(houseMesh);
	house_obj->GetTransform()->SetPosition({ -8.0f, 0 , 0 });
#endif


	//std::shared_ptr<GameObject> nanachi = CreateGameObject();
	//nanachi->AddComponent<MeshRenderer>()->SetMaterial(cubeMaterial);
	//nanachi->AddComponent<MeshFilter>()->SetMesh(nanachiMesh);
	//nanachi->GetTransform()->SetScale({ 0.01f, 0.01f, 0.01f });
	//nanachi->GetTransform()->SetPosition({ 0, 5.0f, 0.0f });

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
	//text->GetTransform()->SetPosition({ 0, 1240, 0 });
	text->GetTransform()->SetScale({ 40, 45, 1 });
	text->GetTransform()->SetEulerAngles({ 0, 0, 0 });
	fpsCounter_->font = inconsolataFont_;

	//text->AddComponent<PlayerMovement>();
	fpsCounter_->onScreen = true;	


	auto terrain = Terrain::GenerateTerrain(
		"Assets/heightmap.png",
		"Assets/grass.png",
		"Assets/blendMap.png",
		"Assets/mud.png",
		"Assets/grassFlowers.png",
		"Assets/path.png",
		4.0f
	);

	if (terrain)
	{
		objects_.push_back(terrain);

		auto& terrain_mat = objects_.back()->GetComponent<MeshFilter>()->GetMesh()->materials.back();
		terrain_mat->tilingFactor = 40.f;

		auto& terrain_mat_data = terrain_mat->data;
		//terrain_mat_data->_colour = {86.f / 255.f, 125.f / 255.f, 70.f / 255.f, 1.0};

		//terrain_mat_data->_diffuse = terrain_mat_data->_colour;
		terrain_mat_data->_specular = { 0.07f, 0.4f, 0.07f, 1.0f };
		terrain_mat_data->_ambient = { 0.15f, 0.15f, 0.15f, 1.0f };
		terrain_mat_data->_shininess = 255.0f;
		//terrain_mat_data->_diffuse = { 0.2f, 0.2f, 0.2f, 1.0f };
		terrain_mat_data->_diffuse = {86.f / 255.f, 125.f / 255.f, 70.f / 255.f, 1.0};

		terrain->GetTransform()->SetPosition({ -400, -60, -400 });
		//terrain->Update(0.01);
		terrain->GetComponent<TerrainCollider>()->Initialize();
	}

	//std::shared_ptr<GameObject> wall = CreateGameObject();
	//wall->AddComponent<MeshRe	
	lanternMesh_ = MeshBuilder::GenerateOBJMTL("Lantern", "Assets/lantern/lantern.obj", "Assets/lantern/lantern.mtl");

	std::vector<glm::vec3> lantern_positions
	{
		{20,  terrain->GetHeight(20, 20), 20},
		{240, terrain->GetHeight(240, 240) + 0.5f, 240 },
		{-50, terrain->GetHeight(-50, 100) + 0.5f, 100 },
		{-59, terrain->GetHeight(-59, -40) + 0.5f, -40 },
	};

	std::vector<glm::vec3> lantern_scales
	{
		{ 4, 4, 4},
		{ 4, 4, 4},
		{ 4, 4, 4},
		{ 4, 4, 4},
	};

	std::vector<glm::vec3> lantern_rotations
	{
		{},
		{0, 90, 0},
		{},
		{},
	};

	std::vector<glm::vec4> lantern_light_colours
	{
		{ 0.8f, 0, 0, 1.0f},
		{ 0.0f, 0.8f, 0.2f, 1.0f},
		{ 0.8f, 0.0f, 0.8f, 1.0f},
		{ 0.f, 0.8f, 0.f, 1.0f},
	};

	for (int i = 0; i < lantern_positions.size(); ++i)
	{
		auto lanternLight = CreateLight();

		lanternLight->data->type = LightType::Point;
		lanternLight->data->cos_cutoff = glm::radians(45.0f);
		lanternLight->data->cos_inner = glm::radians(30.0f);
		lanternLight->data->constant = 0.1f;
		lanternLight->data->linear = 0.01f;
		lanternLight->data->quadratic = 0.001f;

		lanternLight->data->colour = lantern_light_colours[i];

		auto lanternObject = lanternLight->GetGameObject();
		auto mr = lanternObject->AddComponent<MeshRenderer>();
		mr->renderOrder = 0;
		lanternObject->AddComponent<MeshFilter>()->SetMesh(lanternMesh_);
		lanternObject->GetTransform()->SetPosition(lantern_positions[i]);
		lanternObject->GetTransform()->SetScale(lantern_scales[i]);
		lanternObject->GetTransform()->SetEulerAngles(lantern_rotations[i]);
		
		auto col = lanternObject->AddComponent<SphereCollider>();
		col->SetRadius(0.5f);
		col->Update(0);

		auto rb = lanternObject->AddComponent<RigidBody>();
		rb->SetMass(3.0f);
		rb->Update(0);

		lanternObject->AddComponent<ThrowableRock>();
	}


	Material* envMapMat = new Material();
	envMapMat->materialShader = shaderLib_->Get("Environment Map");

	auto reward = CreateGameObject();

	sphereMesh2_ = MeshBuilder::GenerateSphere("Sphere", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, 32, 1.f);
	reward->AddComponent<MeshRenderer>()->SetMaterial(envMapMat);
	reward->AddComponent<MeshFilter>()->SetMesh(sphereMesh2_);
	reward->GetTransform()->SetPosition(glm::vec3(50.f, 50.f, 50.f));
	reward->GetTransform()->SetScale(glm::vec3(3.f));
	reward->AddComponent<SphereCollider>();
	auto rewardrb = reward->AddComponent<RigidBody>();
	rewardrb->SetMass(1.f);
	rewardrb->SetRestitution(0.9f);

	//std::shared_ptr<GameObject> environmentMappedSphere = CreateGameObject();
	//environmentMappedSphere->AddComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMesh(SceneRenderer::GEO_BALL)));
	//environmentMappedSphere->AddComponent<MeshRenderer>()->SetMaterial(envMapMat);
	//environmentMappedSphere->GetTransform()->SetPosition({ 0, 10, 0 });
	//environmentMappedSphere->AddComponent<RigidBody>();
	//environmentMappedSphere->AddComponent<SphereCollider>();
	//environmentMappedSphere->AddComponent<PlayerMovement>();


	playerBase_ = CreateGameObject();
	//playerBase->AddComponent<MeshFilter>()->SetMesh(playerModel[TORSO]);
	//playerBase->AddComponent<MeshRenderer>();
	//playerBase->GetTransform()->SetPosition({ 0, 0, 0 });

	playerBase_->GetTransform()->SetScale({ 1, 1, 1 });
	//environmentMappedSphere->AddComponent<RigidBody>();
	//player->AddComponent<SphereCollider>();
	auto pc = playerBase_->AddComponent<PlayerController>();
	pc->SetTerrain(terrain);
	pc->BindCamera(cameras_[0]);
	pc->BindCrosshair(crosshair);


	//pcc->SetToLookAt(_playerBase->GetTransform());

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

	camera_controller->Follow(playerBase_->GetTransform());
	//pc->CameraFollowTransform(playerHead->GetTransform());
	pc->SetCameraPositionOffset({ 0, 9.5f, 0 });

	flashlightMesh_ = MeshBuilder::GenerateOBJMTL("Flashlight", "Assets/fl.obj", "Assets/fl.mtl");
	flashlightMesh_->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/fl.jpg");

	auto flashlightLight = CreateLight();
	flashlightLight->data->type = LightType::Spot;
	flashlightLight->data->cos_cutoff = glm::radians(45.0f);
	flashlightLight->data->cos_inner = glm::radians(30.0f);
	flashlightLight->data->constant = 0.1f;
	flashlightLight->data->linear = 0.01f;
	flashlightLight->data->quadratic = 0.001f;

	auto flashlight = flashlightLight->GetGameObject();
	flashlight->GetTransform()->SetPosition({100, -10, 50});
	flashlight->AddComponent<MeshRenderer>();
	flashlight->AddComponent<MeshFilter>()->SetMesh(flashlightMesh_);
	flashlight->GetTransform()->SetScale(glm::vec3(0.005f));
	auto sc = flashlight->AddComponent<SpotlightController>();
	sc->BindRay(pc->GetRay());

	flashlight->AddComponent<ThrowableRock>();
	auto flashlightCol = flashlight->AddComponent<SphereCollider>();
	auto flashlightRb = flashlight->AddComponent<RigidBody>();
	flashlightRb->SetMass(8.f);
	flashlightRb->SetRestitution(0.2f);
	flashlightCol->Initialize();
	flashlightCol->SetRadius(0.4f);

	//tentMesh_ = MeshBuilder::GenerateOBJMTL("Tent", "Assets/Carousel/Obj/CarouselTop2.obj", "Assets/Carousel/Obj/CarouselTop2.mtl");
	//tentMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Carousel/Img/CarouselDiffuse.png");
	//for (int i = 1 ; i < 39; ++i)
	//	tentMesh_->materials[i]->textureMaps[Material::DIFFUSE] = tentMesh_->materials[0]->textureMaps[Material::DIFFUSE];
	auto tent = CreateGameObject();
	tent->AddComponent<ModelRenderer>()->LoadModel("Assets/Ahri/source/Ahri/Ahri.obj");
	//tent->AddComponent<MeshFilter>()->SetMesh(tentMesh_);
	tent->GetTransform()->SetPosition(glm::vec3(1.f, -10.f, 1.f));
	tent->GetTransform()->SetScale(glm::vec3(1.f, 1.f, 1.f));


	// if (rockPileMesh_)
	// {
	// 	rockPileMesh_->materials[0]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/001.png");
	// 	rockPileMesh_->materials[1]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/001.png");
	// 	rockPileMesh_->materials[2]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/003.png");
	// 	rockPileMesh_->materials[3]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/003.png");
	// 	rockPileMesh_->materials[4]->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/rockpile/textures/002.png");
	// }


	//std::vector<glm::vec3> rockPilePositions
	//{
	//	{ -40, 0, 50 },
	//	{ 300, 0, -143 },
	//	{ 200, 0, -300 },
	//	{ -100, 0, 68 },
	//	{ -200, 0, 95 },
	//	{ -40, 0, -40 },
	//	{ -120, 0, 305 },
	//	{ 1, 0, 377 }
	//};

	//if (terrain)
	//{
	//	for (int i = 0; i < rockPilePositions.size(); ++i)
	//	{
	//		auto rockPile = CreateGameObject();
	//		rockPile->AddComponent<MeshRenderer>();
	//		auto cr = rockPile->AddComponent<CoveringRock>();

	//		rockPile->AddComponent<MeshFilter>()->SetMesh(rockPileMesh_);
	//		rockPile->GetTransform()->SetScale(glm::vec3(12.f));
	//		rockPile->GetTransform()->SetPosition({ rockPilePositions[i].x, terrain->GetHeight(rockPilePositions[i].x, rockPilePositions[i].z), rockPilePositions[i].z });

	//		if (i == 0)
	//			cr->BindObject(teddyBear);

	//		auto col = rockPile->AddComponent<SphereCollider>();
	//		col->SetRadius(0.6f);
	//		col->Initialize();
	//	}
	//}

	//hutaoMesh = MeshBuilder::GenerateOBJMTL("Hutao", "Assets/hutao.obj", "Assets/hutao.mtl");
	//Texture* hutaoText = Texture::LoadTexture("Assets/hutao.tga");

	//hutaoMesh->materials.front()->textureMaps[Material::DIFFUSE] = hutaoText;

	//std::shared_ptr<GameObject> hutao = CreateGameObject();
	//hutao->AddComponent<MeshFilter>()->SetMesh(hutaoMesh);
	//hutao->AddComponent<MeshRenderer>();
	//hutao->GetTransform()->SetPosition({ 20, 50, 20 });
	//ahriTalkable->BindOutput(dialogueText_);
	//ahriTalkable->BindObject(reward);
	dialogueText_->SetText("Test");

	//Update(0.01);
	sceneCamera_ = cameras_[0];
}

void JungleScene::Update(double dt)
{
	fpsCounter_->SetText("FPS: " + std::to_string(static_cast<int>(1.0 / dt)));
	if (Input::GetInstance()->IsKeyPressed('N') && !isChangingCamera_)
	{
		//static bool player = true;
		static int index = 1;
		SwitchCamera(cameras_[index]);
		++index;
		if (index == NUM_CAMERAS)
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
	}

	CameraInterp();

	static Input* input = Input::GetInstance();
	input->Update();

}

void JungleScene::Render()
{
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

void JungleScene::Exit()
{
	//for (auto& i : objects_)
	//	if (i)
	//	{
	//		delete i;
	//		i = nullptr;
	//	}
	delete quadMesh_;
	quadMesh_ = nullptr;
	delete quadMesh2_;
	quadMesh2_ = nullptr;
	delete sphereMesh_;
	sphereMesh_ = nullptr;
	delete sphereMesh2_;
	sphereMesh2_ = nullptr;

	if (hutaoMesh_)
	{
		delete hutaoMesh_;
		hutaoMesh_ = nullptr;
	}

	delete shaderLib_;
	shaderLib_ = nullptr;


	if (rockMesh_)
	{
		delete rockMesh_;
		rockMesh_ = nullptr;
	}

	if (treeMesh_)
	{
		delete treeMesh_;
		treeMesh_ = nullptr;
	}

	if (skybox_)
	{
		delete skybox_;
		skybox_ = nullptr;
	}

	if (ahriMesh_)
	{
		for (auto& i : ahriMesh_->materials)
		{
			delete i->data;
			i->data = nullptr;

			i = nullptr;
		}

		delete ahriMesh_;
		ahriMesh_ = nullptr;
	}

	delete inconsolataFont_;
	inconsolataFont_ = nullptr;

	delete playerModel_.front()->materials.front()->textureMaps[Material::DIFFUSE];

	for (int i = 0; i < NUM_PLAYER_PARTS; ++i)
	{
		auto& j = playerModel_[i];
		if (!j || !j->materials.empty()) continue;
		j->materials.front()->textureMaps[Material::DIFFUSE] = nullptr;
		delete j;
		j = nullptr;
	}

	delete flashlightMesh_;
	flashlightMesh_ = nullptr;

	delete lanternMesh_;
	lanternMesh_ = nullptr;
}



Light* JungleScene::CreateLight()
{
	std::shared_ptr<GameObject> temp = CreateGameObject();
	auto temp_light = temp->AddComponent<Light>(); 	
	lights_.emplace_back(temp_light->data);
	//lights_[numLights_] = temp_light->data;
	//++numLights_;
	return temp_light;
}

void JungleScene::CameraInterp()
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

void JungleScene::SwitchCamera(Camera* newCamera)
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

JungleScene::~JungleScene()
{
}
