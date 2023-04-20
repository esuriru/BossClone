#include "PoolTableScene.h"

#include "Physics/ColliderManager.h"
#include "Utils/Input.h"
#include "Renderer/MeshBuilder.h"
#include "Components/MeshRenderer.h"
#include "Components/MeshFilter.h"
#include "Components/CameraController.h"
#include "Core/Application.h"

PoolTableScene::PoolTableScene()
{}

PoolTableScene::~PoolTableScene()
{}

void PoolTableScene::Init()
{
	// TODO:: INIT POOL TABLE
	
	// DIMENSIONS OF POOL TABLE:
	// 224 x 112 ( Inner field )
	// 262 x 150 ( Outer frame )
	// The frame is 19 units thick

#pragma region ground
	ground_ = MeshBuilder::GenerateCube("Ground", glm::vec4(1.f), 1.f);
	
	Material* groundMat = new Material();
	groundMat->data->_ambient = glm::vec4(0.3f, 0.3f, 0.3f, 1.f);
	groundMat->data->_diffuse = glm::vec4(1.f);
	groundMat->data->_specular = glm::vec4(0.f);
	groundMat->data->_shininess = 0.f;
	groundMat->materialShader = shaderLib_->GetShaderFromIllumModel(2);
	groundMat->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/PoolTable/ground.jpg");

	ground_->materials.emplace_back(groundMat);

	// Create the ground GO
	std::shared_ptr<GameObject> groundGO = CreateGameObject();
	groundGO->AddComponent<MeshFilter>()->SetMesh(ground_);
	groundGO->AddComponent<MeshRenderer>()->SetMaterial(groundMat);
	groundGO->GetTransform()->SetScale(glm::vec3(100.f, 1.f, 50.f));
#pragma endregion

#pragma region camera
	std::shared_ptr<GameObject> mainCamera = CreateGameObject();
	// Creating an actual camera component in the GameObject and binding it to sceneCamera_
	sceneCamera_ = mainCamera->AddComponent<Camera>();
	// Adding the Camera Controller
	mainCamera->AddComponent<CameraController>();
	// Set the projection matrix
	glm::mat4 projectionMatrix = glm::perspective(45.f, static_cast<float>(Application::GetInstance()->GetWindowWidth()) / static_cast<float>(Application::GetInstance()->GetWindowHeight()), 0.1f, 1000.f);
	sceneCamera_->projection = projectionMatrix;
	sceneCamera_->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.f, 1.f, 0.f));
#pragma endregion

	shaderLib_ = ShaderLibrary::GetInstance();
}

void PoolTableScene::Update(double dt)
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
	}

	static Input* input = Input::GetInstance();
	input->Update();
}

void PoolTableScene::Render()
{
	renderer_ = SceneRenderer::GetInstance();

	// Clear the buffers
	renderer_->Clear();
	renderer_->SetClearColour(glm::vec4(0.0f, 0.0f, 0.4f, 1.f));

	// No skybox hence nullptr
	renderer_->BeginScene(sceneCamera_, ShaderLibrary::GetInstance());


	// Render the GameObjects
	for (const std::shared_ptr<GameObject> i : objects_)
	{
		if (i->ActiveSelf())
		{
			i->Render();
		}
	}

	renderer_->EndScene();
}

void PoolTableScene::Exit()
{
	const auto DeletePtr = [](Mesh* ptr)
	{
		if (ptr)
		{
			delete ptr; 
			ptr = nullptr;
		}
	};

	DeletePtr(ground_);
}