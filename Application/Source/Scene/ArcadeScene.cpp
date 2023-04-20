#include "ArcadeScene.h"

#include <thread>

#include "Renderer/MeshBuilder.h"
#include "Renderer/Texture.h"
#include "Components/MeshRenderer.h"
#include "Components/MeshFilter.h"
#include "Components/PlayerCameraController.h"
#include "Core/Application.h"
#include "Utils/Input.h"
#include "Physics/ColliderManager.h"
#include "Components/ModelRenderer.h"
#include "Components/UIRenderer.h"
#include "Components/PlayerCameraController.h"
#include "Components/ClawMachineController.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Components/RigidBody.h"

ArcadeScene::ArcadeScene()
	: Scene()
{
	applicationRefreshes_ = 8;
}

ArcadeScene::~ArcadeScene()
{}

void ArcadeScene::Init()
{
	// Scale of the environment 
	// The room is 300 x 300.
	// The room is also 100 units high.
	const float xz_scale = 300.f;
	const float vert_scale = 100.f;

#pragma region portal
	Application::GetInstance()->UpdateLoadingScene("Loading portal.");
	meshlist_[GEO_PORTAL] = MeshBuilder::GenerateSphere("Portal", glm::vec4(1.f), 90, 90);
	Material* portalMat = new Material();
	mats_.push_back(portalMat);

	// Setting the ambient, diffuse, specular components to match wood
	// Ambient Tone: BISTRE (3D2B1F)
	portalMat->data->_ambient = glm::vec4(1.f);
	// Diffuse Tone: Wood Brown (C19A6B)
	portalMat->data->_diffuse = glm::vec4(1.f);
	// Specular Tone: Wood polish (Assume is white)
	portalMat->data->_specular = glm::vec4(0.f);
	portalMat->data->_shininess = 0.f;

	// Set the shader
	// Illum model 2 because A, D and S components all present
	portalMat->materialShader = shaderLib_->GetShaderFromIllumModel(1);
	portalMat->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Arcade/portal/obsidian.png");
	// Set tiling factor to make the wall seem more high-quality
	portalMat->tilingFactor = 3;
	// Bind material
	meshlist_[GEO_PORTAL]->materials.emplace_back(portalMat);

	// Create the GO
	std::shared_ptr<GameObject> portal = CreateGameObject();
	portal->AddComponent<UIRenderer>();
	portal->GetComponent<UIRenderer>()->billboarded = true;
	portal->AddComponent<MeshFilter>()->SetMesh(meshlist_[GEO_PORTAL]);
	portal->GetTransform()->SetPosition(glm::vec3(0.f, 30.f, 130.f));
	portal->GetTransform()->SetScale(glm::vec3(20.f, 30.f, 2.f));

	const Light* portalLight = CreateLight();
	portalLight->GetGameObject()->GetTransform()->SetPosition(glm::vec3(portal->GetTransform()->GetPosition().x, portal->GetTransform()->GetPosition().y, portal->GetTransform()->GetPosition().z - 10.f));

	portalLight->data->type = LightType::Point;
	portalLight->data->colour = glm::vec4(1.f, 0.f, 1.f, 1.f);
	portalLight->data->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.f);
	portalLight->data->cos_cutoff = glm::radians(45.0f);
	portalLight->data->cos_inner = glm::radians(30.0f);
	portalLight->data->constant = 0.1f;
	portalLight->data->linear = 0.01f;
	portalLight->data->quadratic = 0.001f;
	portalLight->data->power = 2.f;
#pragma endregion portal

#pragma region wall
		Application::GetInstance()->UpdateLoadingScene("Loading walls.");
		meshlist_[GEO_WALL] = MeshBuilder::GenerateQuad("Wall", glm::vec4(1.f, 1.f, 1.f, 1.f), 1.f);
		// Create material and bind texture
		Material* wallMat = new Material();
		mats_.push_back(wallMat);
		// Setting the ambient, diffuse, specular components to match wood
		// Ambient Tone: BISTRE (3D2B1F)
		wallMat->data->_ambient = glm::vec4((61.f / 255.f), (43.f / 255.f), (31.f / 255.f), 1.f);
		// Diffuse Tone: Wood Brown (C19A6B)
		wallMat->data->_diffuse = glm::vec4((193.f / 255.f), (154.f / 255.f), (107.f / 255.f), 1.f);
		// Specular Tone: Wood polish (Assume is white)
		wallMat->data->_specular = glm::vec4(1.f, 1.f, 1.f, 1.f);
		wallMat->data->_shininess = 5.f;
		// Set the shader
		// Illum model 2 because A, D and S components all present
		wallMat->materialShader = shaderLib_->GetShaderFromIllumModel(2);
		wallMat->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Arcade/Wall/tex.png");
		// Set tiling factor to make the wall seem more high-quality
		wallMat->tilingFactor = 10;
		// Bind material
		meshlist_[GEO_WALL]->materials.emplace_back(wallMat);

		// Create wall GameObjects (4, one per side)
		// Also scale and position accordingly usinng predefined const variables
		for (int i = 0; i < 4; i++)
		{
			// Rotate by 90 deg per i;
			float angle = -90 * i;

			std::shared_ptr<GameObject> wall = CreateGameObject();
			wall->AddComponent<MeshRenderer>()->SetMaterial(wallMat);
			wall->AddComponent<MeshFilter>()->SetMesh(meshlist_[GEO_WALL]);

			// Switch based on i, since they are on different sides
			// We init front, right, back, left due to the order of i's increment ( Change by 90 deg CW per iteration )
			// Also init to y_scale to make it level with y = 0

			const float yScale = vert_scale * 0.5f;

			switch (i)
			{
			case 0:
				wall->GetTransform()->SetPosition(glm::vec3(0.f, yScale, -xz_scale * 0.5f));
				break;
			case 1:
				wall->GetTransform()->SetPosition(glm::vec3(xz_scale * 0.5f, yScale, 0.f));
				break;
			case 2:
				wall->GetTransform()->SetPosition(glm::vec3(0.f, yScale, xz_scale * 0.5f));
				break;
			case 3:
				wall->GetTransform()->SetPosition(glm::vec3(-xz_scale * 0.5f, yScale, 0.f));
				break;
			}
			wall->GetTransform()->SetEulerAngles(glm::vec3(0.f, angle, 0.f));
			wall->GetTransform()->SetScale(glm::vec3(xz_scale, vert_scale, 1.f));
		}
#pragma endregion wall

#pragma region ceiling
		Application::GetInstance()->UpdateLoadingScene("Loading ceiling.");
		// Init mesh
		meshlist_[GEO_CEILING] = MeshBuilder::GenerateQuad("Ceiling", glm::vec4(1.f, 1.f, 1.f, 1.f), 1.f);
		// Create material and bind texture
		Material* ceilingMat = new Material();
		mats_.push_back(ceilingMat);
		// Setting the ambient, diffuse, specular components to match wood
		// Ambient Tone: BISTRE (3D2B1F)
		ceilingMat->data->_ambient = glm::vec4((61.f / 255.f), (43.f / 255.f), (31.f / 255.f), 1.f);
		// Diffuse Tone: Wood Brown (C19A6B)
		ceilingMat->data->_diffuse = glm::vec4((193.f / 255.f), (154.f / 255.f), (107.f / 255.f), 1.f);
		// Specular Tone: Wood polish (Assume is white)
		ceilingMat->data->_specular = glm::vec4(1.f, 1.f, 1.f, 1.f);
		ceilingMat->data->_shininess = 5.f;
		// Set the shader
		// Illum model 2 because A, D and S components all present
		ceilingMat->materialShader = ShaderLibrary::GetShaderFromIllumModel(1);
		ceilingMat->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Arcade/Ceiling/tex.png");
		// Set tiling factor to make the wall seem more high-quality
		ceilingMat->tilingFactor = 10;
		// Bind material
		meshlist_[GEO_CEILING]->materials.emplace_back(ceilingMat);

		// Init the GameObject
		std::shared_ptr<GameObject> ceiling = CreateGameObject();
		ceiling->AddComponent<MeshRenderer>()->SetMaterial(ceilingMat);
		ceiling->AddComponent<MeshFilter>()->SetMesh(meshlist_[GEO_CEILING]);
		ceiling->GetTransform()->SetPosition(glm::vec3(0.f, vert_scale, 0.f));
		ceiling->GetTransform()->SetEulerAngles(glm::vec3(90.f, 90.f, 0.f));
		ceiling->GetTransform()->SetScale(glm::vec3(xz_scale, xz_scale, 1.f));
		
#pragma endregion ceiling

#pragma region floor
		Application::GetInstance()->UpdateLoadingScene("Loading floor.");
		// Init mesh
		meshlist_[GEO_FLOOR] = MeshBuilder::GenerateQuad("Floor", glm::vec4(1.f, 1.f, 1.f, 1.f), 1.f);
		//meshlist_[GEO_FLOOR] = SceneRenderer::GetInstance()->GetMesh(SceneRenderer::GEO_QUAD);

		// Create material and bind texture
		Material* floorMat = new Material();
		mats_.push_back(floorMat);
		// Setting the ambient, diffuse, specular components to match wood
		// Ambient Tone: BISTRE (3D2B1F)
		//floorMat->data->_ambient = glm::vec4((61.f / 255.f), (43.f / 255.f), (31.f / 255.f), 1.f);
		floorMat->data->_ambient = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
		// Diffuse Tone: Wood Brown (C19A6B)
		floorMat->data->_diffuse = glm::vec4((193.f / 255.f), (154.f / 255.f), (107.f / 255.f), 1.f);
		// Specular Tone: Wood polish (Assume is white)
		floorMat->data->_specular = glm::vec4(0.8f, 0.8f, 0.8f, 1.f);
		floorMat->data->_shininess = 10.f;
		// Set the shader
		// Illum model 2 because A, D and S components all present
		floorMat->materialShader = ShaderLibrary::GetShaderFromIllumModel(2);
		floorMat->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Arcade/floor/carpet.png");
		// Set tiling factor to make the wall seem more high-quality
		floorMat->tilingFactor = 10;
		// Bind material
		meshlist_[GEO_FLOOR]->materials.emplace_back(floorMat);

		// Create the GO

		std::shared_ptr<GameObject> floor = CreateGameObject();
		floor->AddComponent<MeshRenderer>()->SetMaterial(floorMat);
		floor->AddComponent<MeshFilter>()->SetMesh(meshlist_[GEO_FLOOR]);
		floor->GetTransform()->SetEulerAngles(glm::vec3(-90.f, 90.f, 0.f));
		floor->GetTransform()->SetScale(glm::vec3(xz_scale, xz_scale, 1.f));
#pragma endregion floor
	
#pragma region clawmachine
		Application::GetInstance()->UpdateLoadingScene("Loading claw machine.");
		// Create its gameobject
		std::shared_ptr<GameObject> clawmachine = CreateGameObject();
		std::vector<Mesh*> clawMachineMeshes = clawmachine->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/ClawMachine/clawmachinebody.obj");
		clawMachineMeshes[1]->materials.front()->data->_diffuse.a = 0.5;
		clawMachineMeshes[3]->materials.front()->data->_diffuse.a = 0.5;
		ModelRenderer* modelRenderer = clawmachine->GetComponent<ModelRenderer>();
		std::swap(modelRenderer->meshes[10], modelRenderer->meshes[1]);
		std::swap(modelRenderer->meshes[11], modelRenderer->meshes[3]);
		
		clawmachine->GetComponent<ModelRenderer>()->renderOrder = 2;
	
		clawmachine->GetTransform()->SetPosition(glm::vec3(-xz_scale * 0.5f + 17.5f, 0.f, -xz_scale * 0.5f + 17.5f));
		clawmachine->GetTransform()->SetScale(glm::vec3(6.f, 6.f, 6.f));

		BoxCollider* clawMachineCol = clawmachine->AddComponent<BoxCollider>();
		clawMachineCol->Initialize();
		clawMachineCol->SetSize({ 18.f, 1.f, 18.f });
		clawMachineCol->SetOffset({ 0.f, 20.f, 0.f });

		// Create gameobject for the claw body
		std::shared_ptr<GameObject> clawBase = CreateGameObject();
		clawBase->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/ClawMachine/clawmachineclawbase.obj");
		clawBase->GetComponent<ModelRenderer>()->renderOrder = 1;
		clawBase->GetTransform()->SetParent(clawmachine->GetTransform());

		SphereCollider* clawCol = clawBase->AddComponent<SphereCollider>();
		clawCol->Initialize();
		clawCol->SetRadius(4.f);
		clawCol->SetOffset({ 0.f, 33.f, 0.f });
		//clawCol->SetActive(false);

		std::shared_ptr<GameObject> clawWall1 = CreateGameObject();
		clawWall1->GetTransform()->SetParent(clawmachine->GetTransform());
		BoxCollider* wallCol1 = clawWall1->AddComponent<BoxCollider>();
		wallCol1->SetSize({ 18.f, 18.f, 1.f });
		wallCol1->SetOffset({ 0.f, 21.f, -9.f });

		std::shared_ptr<GameObject> clawWall2 = CreateGameObject();
		clawWall2->GetTransform()->SetParent(clawmachine->GetTransform());
		BoxCollider* wallCol2 = clawWall2->AddComponent<BoxCollider>();
		wallCol2->SetSize({ 18.f, 18.f, 1.f });
		wallCol2->SetOffset({ 0.f, 21.f, 9.f });

		std::shared_ptr<GameObject> clawWall3 = CreateGameObject();
		clawWall3->GetTransform()->SetParent(clawmachine->GetTransform());
		BoxCollider* wallCol3 = clawWall3->AddComponent<BoxCollider>();
		wallCol3->SetSize({ 1.f, 18.f, 18.f });
		wallCol3->SetOffset({ 9.f, 21.f, 0.f });

		std::shared_ptr<GameObject> clawWall4 = CreateGameObject();
		clawWall4->GetTransform()->SetParent(clawmachine->GetTransform());
		BoxCollider* wallCol4 = clawWall4->AddComponent<BoxCollider>();
		wallCol4->SetSize({ 1.f, 18.f, 18.f });
		wallCol4->SetOffset({ -9.f, 21.f, 0.f });

		// Create gameobjects for the claw fingers (or whatever they're called)
		std::shared_ptr<GameObject> claw1 = CreateGameObject();
		claw1->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/ClawMachine/clawmachineclaw1.obj");
		claw1->GetComponent<ModelRenderer>()->renderOrder = 1;
		claw1->GetTransform()->SetParent(clawBase->GetTransform());
		/*claw1->GetTransform()->SetPosition(glm::vec3(-xz_scale * 0.5f + 17.5f, 0, -xz_scale * 0.5f + 17.5f));
		claw1->GetTransform()->SetEulerAngles(glm::vec3(0.f, 45.f, 0.f));
		claw1->GetTransform()->SetScale(glm::vec3(6.f, 6.f, 6.f));*/

		std::shared_ptr<GameObject> claw2 = CreateGameObject();
		claw2->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/ClawMachine/clawmachineclaw2.obj");
		claw2->GetComponent<ModelRenderer>()->renderOrder = 1;
		claw2->GetTransform()->SetParent(clawBase->GetTransform());
		/*claw2->GetTransform()->SetPosition(glm::vec3(-xz_scale * 0.5f + 17.5f, 0, -xz_scale * 0.5f + 17.5f));
		claw2->GetTransform()->SetEulerAngles(glm::vec3(0.f, 45.f, 0.f));
		claw2->GetTransform()->SetScale(glm::vec3(6.f, 6.f, 6.f));*/

		std::shared_ptr<GameObject> claw3 = CreateGameObject();
		claw3->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/ClawMachine/clawmachineclaw3.obj");
		claw3->GetComponent<ModelRenderer>()->renderOrder = 1;
		claw3->GetTransform()->SetParent(clawBase->GetTransform());
		/*claw3->GetTransform()->SetPosition(glm::vec3(-xz_scale * 0.5f + 17.5f, 0, -xz_scale * 0.5f + 17.5f));
		claw3->GetTransform()->SetEulerAngles(glm::vec3(0.f, 45.f, 0.f));
		claw3->GetTransform()->SetScale(glm::vec3(6.f, 6.f, 6.f));*/

		clawBase->AddComponent<ClawMachineController>();
		

#pragma endregion clawmachine

#pragma region sphere
	
		Application::GetInstance()->UpdateLoadingScene("Loading spheres.");
		meshlist_[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", glm::vec4(1.f), 90, 90, 1.f);

		Material* capsuleMat = new Material();
		mats_.push_back(capsuleMat);
		capsuleMat->data->_ambient = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
		capsuleMat->data->_diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		capsuleMat->data->_specular = glm::vec4(1.f, 1.f, 1.f, 1.f);
		capsuleMat->data->_shininess = 10.f;
		capsuleMat->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Arcade/capsule/tex.jpg");
		capsuleMat->materialShader = ShaderLibrary::GetInstance()->GetShaderFromIllumModel(2);

		meshlist_[GEO_SPHERE]->materials.emplace_back(capsuleMat);

		// Create mesh and GO
		std::shared_ptr<GameObject> sphere = CreateGameObject();
		sphere->AddComponent<MeshRenderer>()->SetMaterial(capsuleMat);
		sphere->AddComponent<MeshFilter>()->SetMesh(meshlist_[GEO_SPHERE]);
		sphere->GetTransform()->SetPosition(glm::vec3(-130.f, 21.f, -130.f));
		sphere->GetTransform()->SetScale(glm::vec3(1.5f));

		sphere->AddComponent<RigidBody>()->SetMass(1.f);
		sphere->AddComponent<SphereCollider>();
		sphere->GetComponent<SphereCollider>()->Initialize();
		sphere->GetComponent<SphereCollider>()->SetRadius(1.5f);

		// Create another GO
		std::shared_ptr<GameObject> sphere2 = CreateGameObject();
		sphere2->AddComponent<MeshRenderer>()->SetMaterial(capsuleMat);
		sphere2->AddComponent<MeshFilter>()->SetMesh(meshlist_[GEO_SPHERE]);
		sphere2->GetTransform()->SetPosition(glm::vec3(-126.f, 21.f, -132.f));
		sphere2->GetTransform()->SetScale(glm::vec3(1.5f));
			  
		sphere2->AddComponent<RigidBody>()->SetMass(1.f);
		sphere2->AddComponent<SphereCollider>();
		sphere2->GetComponent<SphereCollider>()->Initialize();
		sphere2->GetComponent<SphereCollider>()->SetRadius(1.5f);

#pragma endregion sphere

#pragma region pooltable
		Application::GetInstance()->UpdateLoadingScene("Loading pool table.");
		// Create the GO
		std::shared_ptr<GameObject> billards = CreateGameObject();
		billards->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/PoolTable/untitled.obj");
		billards->GetTransform()->SetPosition(glm::vec3(-75.f, 17.5f, 72.5f));
		billards->GetTransform()->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
#pragma endregion pooltable

#pragma region table
		Application::GetInstance()->UpdateLoadingScene("Loading table.");
		// Create the GO
		std::shared_ptr<GameObject> table = CreateGameObject();
		table->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/Table/untitled.obj");
		table->GetTransform()->SetPosition(glm::vec3(125.f, 0.f, -72.5f));
		table->GetTransform()->SetScale(glm::vec3(30.f, 30.f, 30.f));
#pragma endregion table

//#pragma region cabinet
//		// Create the GO
//		std::shared_ptr<GameObject> cabinet = CreateGameObject();
//		std::vector<Mesh*> cabinetMeshes = cabinet->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/Cabinet/untitled.obj");
//		cabinetMeshes[8]->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Arcade/Cabinet/ARCADE.png");
//		cabinet->GetTransform()->SetPosition(glm::vec3(125.f, 0.f, 85.5f));
//		cabinet->GetTransform()->SetEulerAngles(glm::vec3(0.f, -90.f, 0.f));
//		cabinet->GetTransform()->SetScale(glm::vec3(30.f, 30.f, 30.f));
//#pragma endregion cabinet

#pragma region light
		// Init the light 
		const auto InitPointLight = [](const Light* light, const glm::vec3& pos)
		{
			light->GetGameObject()->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/Light/untitled.obj").front()->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Arcade/Light/diffuse.jpg");

			light->GetGameObject()->GetTransform()->SetPosition(pos);
			
			light->GetGameObject()->GetTransform()->SetScale(glm::vec3(50.f));

			light->data->type = LightType::Point;
			light->data->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.f);
			light->data->cos_cutoff = glm::radians(45.0f);
			light->data->cos_inner = glm::radians(30.0f);
			light->data->constant = 0.1f;
			light->data->linear = 0.01f;
			light->data->quadratic = 0.001f;
			light->data->power = 2.f;
		};

		lightY = 100.f - 20.f;
		float lightModelY = 100.f;

		InitPointLight(CreateLight(), glm::vec3(0.f, lightModelY, -75.f));

		InitPointLight(CreateLight(), glm::vec3(-50.f, lightModelY, 80.f));

		InitPointLight(CreateLight(), glm::vec3(80.f, lightModelY, 0.f));

		InitPointLight(CreateLight(), glm::vec3(10.f, lightModelY, 115.f));
#pragma endregion light

#pragma region ddr
		// Create the GO
		std::shared_ptr<GameObject> ddr = CreateGameObject();
		std::vector<Mesh*> meshes = ddr->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/DDR/untitled.obj");
		meshes[0]->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/Arcade/DDR/diffuse.png");

		ddr->GetTransform()->SetPosition(glm::vec3(0.f, 0.f, -115.f));
		ddr->GetTransform()->SetScale(glm::vec3(1000.f));
#pragma endregion ddr

#pragma region fumo
		std::shared_ptr<GameObject> fumo = CreateGameObject();
		fumo->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/plush1/untitled.obj");
		fumo->GetTransform()->SetPosition(glm::vec3(121.f, 25.5f, -50.f));
		fumo->GetTransform()->SetEulerAngles(glm::vec3(0.f, -135.f, 0.f));
		fumo->GetTransform()->SetScale(glm::vec3(25.f));
#pragma endregion fumo

#pragma region kirby
		std::shared_ptr<GameObject> kirby = CreateGameObject();
		kirby->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/kirby/untitled.obj");
		kirby->GetTransform()->SetPosition(glm::vec3(120.f, 26.f, -95.f));
		kirby->GetTransform()->SetEulerAngles(glm::vec3(0.f, -90.f, 0.f));
		kirby->GetTransform()->SetScale(glm::vec3(5.f));
#pragma endregion kirby

#pragma region gris
		std::shared_ptr<GameObject> gris = CreateGameObject();
		gris->AddComponent<ModelRenderer>()->LoadModel("Assets/Arcade/Gris/untitled.obj");
		gris->GetTransform()->SetPosition(glm::vec3(120.f, 26.f, -75.f));
		gris->GetTransform()->SetEulerAngles(glm::vec3(0.f, -100.f, 0.f));
		gris->GetTransform()->SetScale(glm::vec3(20.f));

#pragma endregion gris

#pragma region camera
	std::shared_ptr<GameObject> mainCamera = CreateGameObject();
	// Creating an actual camera component in the GameObject and binding it to sceneCamera_
	sceneCamera_ = mainCamera->AddComponent<Camera>();
	// Adding the Camera Controller
	mainCamera->AddComponent<CameraController>();
	// Set the projection matrix
	glm::mat4 projectionMatrix = glm::perspective(45.f, static_cast<float>(Application::GetInstance()->GetWindowWidth()) / static_cast<float>(Application::GetInstance()->GetWindowHeight()), 0.1f, 1000.f);
	sceneCamera_->projection = projectionMatrix;
	sceneCamera_->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.f, vert_scale * 0.4f, 0.f));
#pragma endregion camera
}

void ArcadeScene::Update(double dt)
{
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

	for (int i = 0; i < lights_.size(); i++)
	{
		lights_[i]->centre.y = lightY;
	}

	static Input* input = Input::GetInstance();
	input->Update();
}

Light* ArcadeScene::CreateLight()
{
	std::shared_ptr<GameObject> temp = CreateGameObject();
	auto temp_light = temp->AddComponent<Light>();
	lights_.emplace_back(temp_light->data);
	return temp_light;
}

void ArcadeScene::Render()
{
	if (Input::GetInstance()->IsKeyDown('B'))
	{
		Application::GetInstance()->LoadScene("MainScene");
		return;
	}
	static SceneRenderer* renderer = SceneRenderer::GetInstance();

	// Clear the buffers
	renderer->Clear();
	renderer->SetClearColour(glm::vec4(0.0f, 0.0f, 0.4f, 1.f));

	// No skybox hence nullptr
	renderer->BeginScene(sceneCamera_, ShaderLibrary::GetInstance(), nullptr, lights_, lights_.size());
	

	// Render the GameObjects
	for (const std::shared_ptr<GameObject> i : objects_)
	{
		if (i->ActiveSelf())
		{
			i->Render();
		}
	}

	renderer->EndScene();
}

void ArcadeScene::Exit()
{
	const auto DeletePtr = [](Mesh* ptr)
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	};

	for (int i = 0; i < TOTAL_MESHES; ++i)
	{
		DeletePtr(meshlist_[i]);
	}

	for (auto& m : mats_)
	{
		delete m;
	}
	mats_.clear();
	lights_.clear();
	objects_.clear();
	//DeletePtr(meshlist_[GEO_WALL]);
	//DeletePtr(meshlist_[GEO_CEILING]);
	//DeletePtr(meshlist_[GEO_FLOOR]);
	//DeletePtr(meshlist_[GEO_SPHERE]);
}