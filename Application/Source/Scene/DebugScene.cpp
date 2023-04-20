#include "DebugScene.h"

#include "Renderer/MeshBuilder.h"
#include "Components/ForceDebug.h"
#include "Components/SphereCollider.h"
#include "Components/BoxCollider.h"
#include "Components/RigidBody.h"
#include "Components/ModelRenderer.h"
#include "Components/MeshRenderer.h"
#include "Components/MeshFilter.h"
#include "Components/CameraController.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Application.h"
#include "Physics/ColliderManager.h"
#include "Utils/Input.h"

#include "Renderer/SceneRenderer.h"

#include <glm/glm.hpp>

DebugScene::DebugScene()
{
	applicationRefreshes_ = 4;
}

DebugScene::~DebugScene()
{
}

void DebugScene::Init()
{
	Application::GetInstance()->UpdateLoadingScene("Creating materials.");
	debugMat_ = std::make_unique<Material>(
		glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
		glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
		glm::vec4(0.9f, 0.9f, 0.9f, 1.0f),
		32.f,
		ShaderLibrary::GetInstance()->Get("Experimental Blinn Phong")
	);

	objectMaterial_ = std::make_unique<Material>(
		glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
		glm::vec4(0.7f, 0.2f, 0.7f, 1.0f),
		glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
		64.f,
		ShaderLibrary::GetInstance()->Get("Experimental Blinn Phong")
	);


	Application::GetInstance()->UpdateLoadingScene("Creating meshes.");
	cubeMesh_ = MeshBuilder::GenerateCube("Cube", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),  0.5f);
	cubeMesh2_ = MeshBuilder::GenerateCube("Cube", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),  0.5f);
	sphereMesh_ = MeshBuilder::GenerateSphere("Sphere", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, 32, 1.f);
	sphereMesh2_ = MeshBuilder::GenerateSphere("Sphere", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, 32, 1.f);

	Application::GetInstance()->UpdateLoadingScene("Creating bodies.");
	ShaderLibrary::GetInstance()->Get("Experimental Blinn Phong")->Bind();
	std::shared_ptr<GameObject> plane = CreateGameObject();
	plane->AddComponent<MeshRenderer>()->SetMaterial(debugMat_.get());
	plane->AddComponent<MeshFilter>()->SetMesh(cubeMesh_);
	plane->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	plane->GetTransform()->SetScale(glm::vec3(100, 5, 100));
	plane->AddComponent<BoxCollider>()->Initialize();

	std::shared_ptr<GameObject> sphere = CreateGameObject();
	sphere->AddComponent<MeshRenderer>()->SetMaterial(debugMat_.get());
	sphere->AddComponent<MeshFilter>()->SetMesh(sphereMesh_);
	sphere->GetTransform()->SetPosition(glm::vec3(0, 5, 0));
	sphere->GetTransform()->SetScale(glm::vec3(3, 3, 3));
	sphere->AddComponent<SphereCollider>()->Initialize();

	std::shared_ptr<GameObject> box = CreateGameObject();
	box->AddComponent<MeshRenderer>()->SetMaterial(objectMaterial_.get());
	box->AddComponent<MeshFilter>()->SetMesh(cubeMesh2_);
	box->GetTransform()->SetPosition(glm::vec3(20, 20, -30));
	box->GetTransform()->SetScale(glm::vec3(21, 3, 7));
	box->AddComponent<BoxCollider>()->Initialize();
	box->AddComponent<RigidBody>()->SetMass(10.f);

	std::shared_ptr<GameObject> box2 = CreateGameObject();
	box2->AddComponent<MeshRenderer>()->SetMaterial(debugMat_.get());
	box2->AddComponent<MeshFilter>()->SetMesh(sphereMesh_);
	box2->GetTransform()->SetPosition(glm::vec3(20, 30, -30));
	box2->GetTransform()->SetScale(glm::vec3(5, 5, 5));
	box2->AddComponent<SphereCollider>()->Initialize();
	box2->AddComponent<RigidBody>()->SetMass(3.0f);
	box2->AddComponent<ForceDebug>();

	std::shared_ptr<GameObject> sphere2 = CreateGameObject();
	sphere2->AddComponent<MeshRenderer>()->SetMaterial(debugMat_.get());
	sphere2->AddComponent<MeshFilter>()->SetMesh(sphereMesh_);
	sphere2->GetTransform()->SetPosition(glm::vec3(20, 40, -30));
	sphere2->GetTransform()->SetScale(glm::vec3(3, 3, 3));
	sphere2->AddComponent<SphereCollider>()->Initialize();
	sphere2->AddComponent<RigidBody>()->SetMass(3.0f);

	//std::shared_ptr<GameObject> axes = CreateGameObject();
	//axes->AddComponent<MeshRenderer>()->SetMaterial(debugMat_.get());
	//axes->AddComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMesh(SceneRenderer::GEO_AXES));
	//std::shared_ptr<GameObject> box3 = CreateGameObject();
	//box3->AddComponent<MeshRenderer>()->SetMaterial(debugMat_.get());
	//box3->AddComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMesh(SceneRenderer::GEO_CUBE)));
	//box3->GetTransform()->SetPosition(glm::vec3(20, 40, -30));
	//box3->GetTransform()->SetScale(glm::vec3(3, 3, 3));
	//box3->AddComponent<BoxCollider>()->Initialize();
	//box3->AddComponent<RigidBody>()->SetMass(0.5f);

	const auto dirlight = CreateLight();
	dirlight->data->direction = { 0.f, -sqrt(2), sqrt(2), 0.f };
	dirlight->data->power = 0.2f;
	//GameObject* lightObject = dirlight->Set
	//lightObject->GetTransform()->SetPosition({ 0, 0, 10 });
	//lightObject->GetComponent<Light>();

	Application::GetInstance()->UpdateLoadingScene("Creating camera.");
	glm::mat4 projectionMatrix = glm::perspective(45.0f, static_cast<float>(Application::GetInstance()->GetWindowWidth())
		/ static_cast<float>(Application::GetInstance()->GetWindowHeight()), 0.1f, 1000.0f);
	std::shared_ptr<GameObject> debugCameraObject = CreateGameObject();
	sceneCamera_ = debugCameraObject->AddComponent<Camera>();
	debugCameraObject->AddComponent<CameraController>();
	sceneCamera_->projection = projectionMatrix;
	debugCameraObject->GetTransform()->SetPosition(glm::vec3(6));

	//auto model = CreateGameObject();
	//model->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	//model->AddComponent<ModelRenderer>()->LoadModel("Assets/Tree 02/Tree.obj");

	//auto model1 = CreateGameObject();
	//model1->GetTransform()->SetPosition(glm::vec3(10, 0, 0));
	////this is cos the blender transform is at 1, 1, 0.142. 
	////but when the fbx was exported the scale was 1, 1, 1 
	//model1->GetTransform()->SetScale(glm::vec3(1, 1, 0.142)); 
	//model1->GetTransform()->SetEulerAngles(glm::vec3(0, 180, 0)); 
	////model1->AddComponent<ModelRenderer>()->LoadModel("Assets/Ahri/source/Ahri/Ahri.obj");
	//model1->AddComponent<ModelRenderer>()->LoadModel("Assets/carousel/source/carousel1.fbx");
	//std::cout << model1->GetTransform()->GetScale().x << "," << model1->GetTransform()->GetScale().y << "," << model1->GetTransform()->GetScale().z << std::endl;
	CC_TRACE("test");
}

void DebugScene::Update(double dt)
{
	static double timePassed = 0;
	timePassed += dt;

	if (Input::GetInstance()->IsKeyPressed('N'))
	{
		Application::GetInstance()->LoadScene("MainScene");
		return;
	}

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

	static Input* input = Input::GetInstance();
	input->Update();
}

void DebugScene::Render()
{
	static SceneRenderer* renderer = SceneRenderer::GetInstance();

	renderer->Clear();
	renderer->SetClearColour({ 189.f / 255.f, 212.f / 255.f, 230.f / 255.f, 0.0f });

	renderer->BeginScene(sceneCamera_, ShaderLibrary::GetInstance(), nullptr, lights_, lights_.size());

	for (const auto& i : objects_)
		if (i->ActiveSelf())
			i->Render();

	renderer->EndScene();
}

void DebugScene::Exit()
{
	delete testMesh_;
	testMesh_ = nullptr;
	delete cubeMesh_;
	cubeMesh_ = nullptr;
	delete cubeMesh2_;
	cubeMesh2_ = nullptr;
	delete sphereMesh_;
	sphereMesh_ = nullptr;
	delete sphereMesh2_;
	sphereMesh2_ = nullptr;

	lights_.clear();
	objects_.clear();
}

Light* DebugScene::CreateLight()
{
	std::shared_ptr<GameObject> temp = CreateGameObject();
	auto temp_light = temp->AddComponent<Light>(); 	
	lights_.emplace_back(temp_light->data);
	return temp_light;
}
