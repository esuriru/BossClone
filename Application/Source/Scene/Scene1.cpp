#include "Scene1.h"
#include "KeyboardController.h"
#include "Components/Droppable.h"
#include "Utils/Input.h"
#include "Utils/LoadTGA.h"
#include "Components/Component.h"
#include "Physics/ColliderManager.h"
#include <glad/glad.h>
#include "Core/Application.h"
#include <glm/glm.hpp>
#include "Renderer/SceneRenderer.h"

#include "Components/RandomDroppable.h"
#include "Components/Droppable.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/MeshFilter.h"
#include "Components/BoxCollider2D.h"
#include "Components/RigidBody2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/PolygonCollider2D.h"
#include "Components/PlayerMovement.h"
#include "Components/FallingObject.h"
#include "Components/BoostPlatform.h"
#include "Components/FinishPoint.h"
#include <glm/gtc/matrix_transform.hpp>


Scene1::Scene1()
{
}

Scene1::~Scene1()
{
	
}

void Scene1::Init()
{
	//Calculating aspect ratio
	shaderLib_ = new ShaderLibrary();
	//Shader::DefaultShader = _shaderLib->Load("Unlit", "Shader/unlitshader.vertexshader", "Shader/unlitshader.fragmentshader");
	shaderLib_->RegisterUniformBuffer("Material");

	worldHeight_ = 500.f;
	worldWidth_m = worldHeight_ * static_cast<float>(Application::GetInstance()->GetWindowWidth()) / Application::GetInstance()->GetWindowHeight();
	GameObject* cameraObject = CreateGameObject();
	sceneCamera_ = cameraObject->AddComponent<Camera>();
	sceneCamera_->projection = glm::ortho(0.f, worldWidth_m, 0.f, worldHeight_, -10.f, 10.f);
	sceneCamera_->Init(
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	Math::InitRNG();


	yellowMat_ = new Material(
		glm::vec4( 1.0f, 215.f/255.f, 0, 1 ),
		shaderLib_->Get("Unlit")
	);

	playerMat_ = new Material(
		glm::vec4(1.0f, 1.0f, 1.0f, 0.5f),
		shaderLib_->Get("Unlit")
	);

#pragma region GameObject_Setup
	GameObject* player = CreateGameObject();
	player->AddComponent<MeshRenderer>();
	player->GetComponent<MeshRenderer>()->SetMaterial(playerMat_);
	player->AddComponent<MeshFilter>();
	player->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_QUAD]);
	player->GetTransform()->SetPosition(glm::vec3(worldWidth_m * 0.5f + 100, 500, 0));
	player->GetTransform()->SetEulerAngles(glm::vec3(0, 0, 0.f));
	player->GetTransform()->SetScale(glm::vec3(50, 50, 9));
	player->AddComponent<BoxCollider2D>();
	player->AddComponent<RigidBody2D>();
	//player->GetComponent<RigidBody2D>()->AddForce({ -10, 0, 0 }, RigidBody2D::FORCE_MODE::IMPULSE);
	player->GetComponent<RigidBody2D>()->SetRestitution(0.8f);
	player->GetComponent<RigidBody2D>()->SetMass(5.0f);
	drops_.push_back(player->AddComponent<Droppable>());


	GameObject* player2 = CreateGameObject();
	player2->AddComponent<MeshRenderer>();
	player2->GetComponent<MeshRenderer>()->SetMaterial(playerMat_);
	player2->AddComponent<MeshFilter>();
	player2->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_BALL2]);
	player2->GetTransform()->SetPosition(glm::vec3(worldWidth_m * 0.5f + 100, 500, 0));
	player2->GetTransform()->SetEulerAngles(glm::vec3(0, 0, 0.f));
	player2->GetTransform()->SetScale(glm::vec3(30, 30, 9));
	player2->AddComponent<CircleCollider2D>();
	player2->AddComponent<RigidBody2D>();
	player2->GetComponent<RigidBody2D>()->SetRestitution(0.8f);
	player2->GetComponent<RigidBody2D>()->SetMass(6.0f);
	drops_.push_back(player2->AddComponent<Droppable>());

	GameObject* player3 = CreateGameObject();
	player3->AddComponent<MeshRenderer>();
	player3->GetComponent<MeshRenderer>()->SetMaterial(playerMat_);
	player3->AddComponent<MeshFilter>();
	player3->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_TRIANGLE]);
	player3->GetTransform()->SetPosition(glm::vec3(worldWidth_m * 0.5f + 100, 500, 0));
	player3->GetTransform()->SetEulerAngles(glm::vec3(0, 0, 0.f));
	player3->GetTransform()->SetScale(glm::vec3(50, 50, 9));
	player3->AddComponent<PolygonCollider2D>()->SetToTriangleCollider();
	player3->AddComponent<RigidBody2D>();
	player3->GetComponent<RigidBody2D>()->SetRestitution(0.8f);
	player3->GetComponent<RigidBody2D>()->SetMass(5.0f);
	drops_.push_back(player3->AddComponent<Droppable>());

	for (int i = 0; i < 3; ++i)
	{
		GameObject* randomable = CreateGameObject();
		randomable->AddComponent<MeshRenderer>();
		randomable->GetComponent<MeshRenderer>()->SetMaterial(playerMat_);
		randomable->AddComponent<MeshFilter>();
		randomable->GetComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_BALL2]));
		toCleanUp_.emplace_back(randomable->GetComponent<MeshFilter>()->GetMesh());
		//randomable->GetTransform()->SetPosition(glm::vec3(m_worldWidth * 0.5f + 100, 500, 0));
		randomable->GetTransform()->SetEulerAngles(glm::vec3(0, 0, 0.f));
		randomable->GetTransform()->SetScale(glm::vec3(15, 15, 9));
		randomable->AddComponent<CircleCollider2D>();
		randomable->AddComponent<RigidBody2D>()->SetActive(false);
		randomable->GetComponent<RigidBody2D>()->SetRestitution(0.8f);
		randomable->GetComponent<RigidBody2D>()->SetMass(2.0f);
		auto r = randomable->AddComponent<RandomDroppable>();
		r->lowerLimit = 60.f + i * 50.f;
		r->upperLimit = 60.f + i * 100.f;
	}

	for (int i = 0; i < 2; ++i)
	{
		GameObject* randomable = CreateGameObject();
		randomable->AddComponent<MeshRenderer>();
		randomable->GetComponent<MeshRenderer>()->SetMaterial(playerMat_);
		randomable->AddComponent<MeshFilter>();
		randomable->GetComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_QUAD]));
		toCleanUp_.emplace_back(randomable->GetComponent<MeshFilter>()->GetMesh());
		//randomable->GetTransform()->SetPosition(glm::vec3(m_worldWidth * 0.5f + 100, 500, 0));
		randomable->GetTransform()->SetScale(glm::vec3(25, 25, 9));
		randomable->AddComponent<BoxCollider2D>();
		randomable->AddComponent<RigidBody2D>()->SetActive(false);
		randomable->GetComponent<RigidBody2D>()->SetRestitution(0.8f);
		randomable->GetComponent<RigidBody2D>()->SetMass(2.0f);
		auto r = randomable->AddComponent<RandomDroppable>();
		r->lowerLimit = 375.f + i * 50.f;
		r->upperLimit = 375.f + i * 100.f;
	}

	GameObject* randomable = CreateGameObject();
	randomable->AddComponent<MeshRenderer>();
	randomable->GetComponent<MeshRenderer>()->SetMaterial(playerMat_);
	randomable->AddComponent<MeshFilter>();
	randomable->GetComponent<MeshFilter>()->SetMesh(new Mesh(*SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_TRIANGLE]));
	toCleanUp_.emplace_back(randomable->GetComponent<MeshFilter>()->GetMesh());
	randomable->GetTransform()->SetScale(glm::vec3(30, 30, 9));
	randomable->AddComponent<PolygonCollider2D>()->SetToTriangleCollider();
	randomable->AddComponent<RigidBody2D>()->SetActive(false);
	randomable->GetComponent<RigidBody2D>()->SetRestitution(0.8f);
	randomable->GetComponent<RigidBody2D>()->SetMass(2.0f);
	auto r = randomable->AddComponent<RandomDroppable>();
	r->lowerLimit = 575.f;
	r->upperLimit = 620.f;
	
	//player->AddComponent<PlayerMovement>();

//	for (int i = 0; i < 3; ++i)
//	{
//		GameObject* fallingTriangle = CreateGameObject();
//		fallingTriangle->AddComponent<MeshRenderer>();
//		fallingTriangle->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//		fallingTriangle->AddComponent<MeshFilter>();
//		fallingTriangle->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_TRIANGLE]);
//		fallingTriangle->GetTransform()->SetPosition(glm::vec3(400, 50, 0));
//		fallingTriangle->GetTransform()->SetScale(glm::vec3(20, 20, 9));
//		fallingTriangle->AddComponent<PolygonCollider2D>();
//		fallingTriangle->GetComponent<PolygonCollider2D>()->SetToTriangleCollider();
//		fallingTriangle->AddComponent<RigidBody2D>();
//		//circle2->GetComponent<RigidBody2D>()->AddForce({ -10, 0, 0 }, RigidBody2D::FORCE_MODE::IMPULSE);
//		fallingTriangle->GetComponent<RigidBody2D>()->SetRestitution(0.3f);
//		//fallingTriangle->AddComponent<FallingObject>();
//		//fallingTriangle->GetComponent<FallingObject>()->lifetime = Math::RandFloatMinMax(2.5f, 5.0f);
//	}
//
//	for (int i = 0; i < 3; ++i)
//	{
//		GameObject* fallingSquare = CreateGameObject();
//		fallingSquare->AddComponent<MeshRenderer>();
//		fallingSquare->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//		fallingSquare->AddComponent<MeshFilter>();
//		fallingSquare->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//		fallingSquare->GetTransform()->SetPosition(glm::vec3(400, 50, 0));
//		fallingSquare->GetTransform()->SetScale(glm::vec3(20, 20, 9));
//		fallingSquare->AddComponent<BoxCollider2D>();
//		fallingSquare->GetComponent<BoxCollider2D>();
//		fallingSquare->AddComponent<RigidBody2D>();
//		fallingSquare->GetComponent<RigidBody2D>()->SetRestitution(0.3f);
//		//fallingSquare->AddComponent<FallingObject>();
//		//fallingSquare->GetComponent<FallingObject>()->lifetime = Math::RandFloatMinMax(2.5f, 5.0f);
//	}
//
//
//	GameObject* triangle = CreateGameObject();
//	triangle->AddComponent<MeshRenderer>();
//	triangle->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	triangle->AddComponent<MeshFilter>();
//	triangle->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_TRIANGLE]);
//	triangle->GetTransform()->SetPosition(glm::vec3( 50, 270, 0));
//	triangle->GetTransform()->SetScale(glm::vec3(15, 15, 3));
//	triangle->AddComponent<PolygonCollider2D>();
//	triangle->GetComponent<PolygonCollider2D>()->SetToTriangleCollider();
//	triangle->AddComponent<RigidBody2D>();
//	//triangle->GetComponent<RigidBody2D>()->AddForce({ 0, 0, 0 }, RigidBody2D::FORCE_MODE::IMPULSE);
//	//triangle->AddComponent<PlayerMovement>();
//
//#pragma region Boundaries
	//GameObject* floor = CreateGameObject();
	//floor->AddComponent<MeshRenderer>();
	//floor->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
	//floor->AddComponent<MeshFilter>();
	//floor->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
	//floor->GetTransform()->SetPosition({0, 10, 0});
	//floor->GetTransform()->SetScale({ 2500, 50, 10});
	////floor->GetTransform()->SetEulerAngles({ 0, 0, 2 });
	//floor->AddComponent<BoxCollider2D>();
	//floor->GetComponent<BoxCollider2D>()->isTrigger = true;

	for (int i = 0; i < 5; ++i)
	{
		GameObject* floor2 = CreateGameObject();
		floor2->AddComponent<MeshRenderer>();
		floor2->GetComponent<MeshRenderer>()->SetMaterial(yellowMat_);
		floor2->AddComponent<MeshFilter>();
		floor2->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_BALL]);
		floor2->GetTransform()->SetPosition({ 150 * i + 35, 100, 0});
		floor2->GetTransform()->SetScale({ 30, 30, 10});
		floor2->AddComponent<CircleCollider2D>();
		floor2->GetComponent<CircleCollider2D>()->isTrigger = true;
	}

	for (int i = 0; i < 4; ++i)
	{
		GameObject* floor2 = CreateGameObject();
		floor2->AddComponent<MeshRenderer>();
		floor2->GetComponent<MeshRenderer>()->SetMaterial(yellowMat_);
		floor2->AddComponent<MeshFilter>();
		floor2->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_BALL]);
		floor2->GetTransform()->SetPosition({ 150 * i + 105, 225, 0});
		floor2->GetTransform()->SetScale({ 30, 30, 10});
		floor2->AddComponent<CircleCollider2D>();
		floor2->GetComponent<CircleCollider2D>()->isTrigger = true;
	}

	for (int i = 0; i < 5; ++i)
	{
		GameObject* floor2 = CreateGameObject();
		floor2->AddComponent<MeshRenderer>();
		floor2->GetComponent<MeshRenderer>()->SetMaterial(yellowMat_);
		floor2->AddComponent<MeshFilter>();
		floor2->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_BALL]);
		floor2->GetTransform()->SetPosition({ 150 * i + 35, 350, 0});
		floor2->GetTransform()->SetScale({ 30, 30, 10});
		floor2->AddComponent<CircleCollider2D>();
		floor2->GetComponent<CircleCollider2D>()->isTrigger = true;
	}

	//GameObject* floor3 = CreateGameObject();
	//floor3->AddComponent<MeshRenderer>();
	//floor3->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
	//floor3->AddComponent<MeshFilter>();
	//floor3->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_BALL]);
	//floor3->GetTransform()->SetPosition({m_worldWidth * 0.72f, 100, 0});
	//floor3->GetTransform()->SetScale({ 50, 50, 10});
	////flo3or->GetTransform()->SetEulerAngles({ 0, 0, 2 });
	//floor3->AddComponent<CircleCollider2D>();
	//floor3->GetComponent<CircleCollider2D>()->isTrigger = true;


	GameObject* wall1 = CreateGameObject();
	wall1->AddComponent<MeshRenderer>();
	wall1->GetComponent<MeshRenderer>()->SetMaterial(yellowMat_);
	wall1->AddComponent<MeshFilter>();
	wall1->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
	wall1->GetTransform()->SetPosition({-225, 500, 0});
	wall1->GetTransform()->SetScale({ 500, 1000, 10});
	wall1->AddComponent<BoxCollider2D>();

	GameObject* wall2 = CreateGameObject();
	wall2->AddComponent<MeshRenderer>();
	wall2->GetComponent<MeshRenderer>()->SetMaterial(yellowMat_);
	wall2->AddComponent<MeshFilter>();
	wall2->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
	wall2->GetTransform()->SetPosition({667.f + 225, 500, 0});
	wall2->GetTransform()->SetScale({ 500, 1000, 10});
	wall2->AddComponent<BoxCollider2D>();
//#pragma endregion Boundaries
//
//#pragma region Platforms
//	GameObject* platform = CreateGameObject();
//	platform->AddComponent<MeshRenderer>();
//	platform->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform->AddComponent<MeshFilter>();
//	platform->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform->GetTransform()->SetPosition({100, 80, 0});
//	platform->GetTransform()->SetScale({ 50, 20, 10});
//	platform->GetTransform()->SetEulerAngles({ 0, 0, -2 });
//	platform->AddComponent<BoxCollider2D>();
//	//auto platBoxCol = platform->GetComponent<BoxCollider2D>();
//
//	GameObject* platformTrigger = CreateGameObject();
//	platformTrigger->AddComponent<MeshRenderer>();
//	platformTrigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platformTrigger->AddComponent<MeshFilter>();
//	platformTrigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platformTrigger->GetTransform()->SetPosition({100, 92, 0});
//	platformTrigger->GetTransform()->SetScale({ 50, 3, 10});
//	platformTrigger->GetTransform()->SetEulerAngles({ 0, 0, -2 });
//	platformTrigger->AddComponent<BoxCollider2D>();
//	platformTrigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform2 = CreateGameObject();
//	platform2->AddComponent<MeshRenderer>();
//	platform2->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform2->AddComponent<MeshFilter>();
//	platform2->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform2->GetTransform()->SetPosition({250, 80, 0});
//	platform2->GetTransform()->SetScale({ 50, 20, 10});
//	platform2->GetTransform()->SetEulerAngles({ 0, 0, 20 });
//	platform2->AddComponent<BoxCollider2D>();
//
//	GameObject* platform2Trigger = CreateGameObject();
//	platform2Trigger->AddComponent<MeshRenderer>();
//	platform2Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform2Trigger->AddComponent<MeshFilter>();
//	platform2Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform2Trigger->GetTransform()->SetPosition({248, 92, 0});
//	platform2Trigger->GetTransform()->SetScale({ 50, 3, 10});
//	platform2Trigger->GetTransform()->SetEulerAngles({ 0, 0, 20 });
//	platform2Trigger->AddComponent<BoxCollider2D>();
//	platform2Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform3 = CreateGameObject();
//	platform3->AddComponent<MeshRenderer>();
//	platform3->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform3->AddComponent<MeshFilter>();
//	platform3->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform3->GetTransform()->SetPosition({400, 80, 0});
//	platform3->GetTransform()->SetScale({ 50, 20, 10});
//	platform3->AddComponent<BoxCollider2D>();
//
//	GameObject* platform3Trigger = CreateGameObject();
//	platform3Trigger->AddComponent<MeshRenderer>();
//	platform3Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform3Trigger->AddComponent<MeshFilter>();
//	platform3Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform3Trigger->GetTransform()->SetPosition({400, 92, 0});
//	platform3Trigger->GetTransform()->SetScale({ 48, 3, 10});
//	platform3Trigger->AddComponent<BoxCollider2D>();
//	platform3Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform4 = CreateGameObject();
//	platform4->AddComponent<MeshRenderer>();
//	platform4->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform4->AddComponent<MeshFilter>();
//	platform4->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform4->GetTransform()->SetPosition({550, 80, 0});
//	platform4->GetTransform()->SetScale({ 50, 20, 10});
//	platform4->GetTransform()->SetEulerAngles({ 0, 0, -20});
//	platform4->AddComponent<BoxCollider2D>();
//
//	GameObject* platform4Trigger = CreateGameObject();
//	platform4Trigger->AddComponent<MeshRenderer>();
//	platform4Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform4Trigger->AddComponent<MeshFilter>();
//	platform4Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform4Trigger->GetTransform()->SetPosition({555, 91, 0});
//	platform4Trigger->GetTransform()->SetScale({ 45, 3, 10});
//	platform4Trigger->GetTransform()->SetEulerAngles({ 0, 0, -20});
//	platform4Trigger->AddComponent<BoxCollider2D>();
//	platform4Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform5 = CreateGameObject();
//	platform5->AddComponent<MeshRenderer>();
//	platform5->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform5->AddComponent<MeshFilter>();
//	platform5->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform5->GetTransform()->SetPosition({ 50, 160, 0});
//	platform5->GetTransform()->SetScale({ 100, 20, 10});
//	platform5->AddComponent<BoxCollider2D>();
//
//	GameObject* platform5Trigger = CreateGameObject();
//	platform5Trigger->AddComponent<MeshRenderer>();
//	platform5Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform5Trigger->AddComponent<MeshFilter>();
//	platform5Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform5Trigger->GetTransform()->SetPosition({ 50, 170, 0});
//	platform5Trigger->GetTransform()->SetScale({ 98, 3, 10});
//	platform5Trigger->AddComponent<BoxCollider2D>();
//	platform5Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform6 = CreateGameObject();
//	platform6->AddComponent<MeshRenderer>();
//	platform6->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform6->AddComponent<MeshFilter>();
//	platform6->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform6->GetTransform()->SetPosition({ m_worldWidth * 0.5f - 100, 240, 0});
//	platform6->GetTransform()->SetScale({ 100, 20, 10});
//	platform6->GetTransform()->SetEulerAngles({ 0, 0, 10});
//	platform6->AddComponent<BoxCollider2D>();
//
//	GameObject* platform6Trigger = CreateGameObject();
//	platform6Trigger->AddComponent<MeshRenderer>();
//	platform6Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform6Trigger->AddComponent<MeshFilter>();
//	platform6Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform6Trigger->GetTransform()->SetPosition({ m_worldWidth * 0.5f - 100, 250, 0});
//	platform6Trigger->GetTransform()->SetScale({ 98, 3, 10});
//	platform6Trigger->GetTransform()->SetEulerAngles({ 0, 0, 10 });
//	platform6Trigger->AddComponent<BoxCollider2D>();
//	platform6Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform7 = CreateGameObject();
//	platform7->AddComponent<MeshRenderer>();
//	platform7->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform7->AddComponent<MeshFilter>();
//	platform7->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform7->GetTransform()->SetPosition({ m_worldWidth * 0.5f - 220, 320, 0});
//	platform7->GetTransform()->SetScale({ 100, 20, 10});
//	platform7->GetTransform()->SetEulerAngles({ 0, 0, -10});
//	platform7->AddComponent<BoxCollider2D>();
//
//	GameObject* platform7Trigger = CreateGameObject();
//	platform7Trigger->AddComponent<MeshRenderer>();
//	platform7Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform7Trigger->AddComponent<MeshFilter>();
//	platform7Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform7Trigger->GetTransform()->SetPosition({ m_worldWidth * 0.5f- 220, 330, 0});
//	platform7Trigger->GetTransform()->SetScale({ 98, 3, 10});
//	platform7Trigger->GetTransform()->SetEulerAngles({ 0, 0, -10 });
//	platform7Trigger->AddComponent<BoxCollider2D>();
//	platform7Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform8 = CreateGameObject();
//	platform8->AddComponent<MeshRenderer>();
//	platform8->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform8->AddComponent<MeshFilter>();
//	platform8->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform8->GetTransform()->SetPosition({ m_worldWidth * 0.5f - 340, 400, 0});
//	platform8->GetTransform()->SetScale({ 100, 20, 10});
//	platform8->GetTransform()->SetEulerAngles({ 0, 0, -10});
//	platform8->AddComponent<BoxCollider2D>();
//
//	GameObject* platform8Trigger = CreateGameObject();
//	platform8Trigger->AddComponent<MeshRenderer>();
//	platform8Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform8Trigger->AddComponent<MeshFilter>();
//	platform8Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform8Trigger->GetTransform()->SetPosition({ m_worldWidth * 0.5f- 340, 410, 0});
//	platform8Trigger->GetTransform()->SetScale({ 98, 3, 10});
//	platform8Trigger->GetTransform()->SetEulerAngles({ 0, 0, -10 });
//	platform8Trigger->AddComponent<BoxCollider2D>();
//	platform8Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform9 = CreateGameObject();
//	platform9->AddComponent<MeshRenderer>();
//	platform9->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform9->AddComponent<MeshFilter>();
//	platform9->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform9->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 320, 160, 0});
//	platform9->GetTransform()->SetScale({ 100, 20, 10});
//	platform9->AddComponent<BoxCollider2D>();
//
//	GameObject* platform9Trigger = CreateGameObject();
//	platform9Trigger->AddComponent<MeshRenderer>();
//	platform9Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform9Trigger->AddComponent<MeshFilter>();
//	platform9Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform9Trigger->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 320, 170, 0});
//	platform9Trigger->GetTransform()->SetScale({ 98, 3, 10});
//	platform9Trigger->AddComponent<BoxCollider2D>();
//	platform9Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform10 = CreateGameObject();
//	platform10->AddComponent<MeshRenderer>();
//	platform10->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform10->AddComponent<MeshFilter>();
//	platform10->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform10->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 200, 220, 0});
//	platform10->GetTransform()->SetScale({ 25, 20, 10});
//	platform10->AddComponent<BoxCollider2D>();
//
//	GameObject* platform10Trigger = CreateGameObject();
//	platform10Trigger->AddComponent<MeshRenderer>();
//	platform10Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform10Trigger->AddComponent<MeshFilter>();
//	platform10Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform10Trigger->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 200, 230, 0});
//	platform10Trigger->GetTransform()->SetScale({ 23, 3, 10});
//	platform10Trigger->AddComponent<BoxCollider2D>();
//	platform10Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform11 = CreateGameObject();
//	platform11->AddComponent<MeshRenderer>();
//	platform11->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform11->AddComponent<MeshFilter>();
//	platform11->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform11->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 300, 280, 0});
//	platform11->GetTransform()->SetScale({ 25, 20, 10});
//	platform11->AddComponent<BoxCollider2D>();
//
//	GameObject* platform11Trigger = CreateGameObject();
//	platform11Trigger->AddComponent<MeshRenderer>();
//	platform11Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform11Trigger->AddComponent<MeshFilter>();
//	platform11Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform11Trigger->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 300, 290, 0});
//	platform11Trigger->GetTransform()->SetScale({ 23, 3, 10});
//	platform11Trigger->AddComponent<BoxCollider2D>();
//	platform11Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform12 = CreateGameObject();
//	platform12->AddComponent<MeshRenderer>();
//	platform12->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform12->AddComponent<MeshFilter>();
//	platform12->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform12->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 210, 340, 0});
//	platform12->GetTransform()->SetScale({ 25, 20, 10});
//	platform12->GetTransform()->SetEulerAngles({ 0, 0, -10});
//	platform12->AddComponent<BoxCollider2D>();
//
//	GameObject* platform12Trigger = CreateGameObject();
//	platform12Trigger->AddComponent<MeshRenderer>();
//	platform12Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform12Trigger->AddComponent<MeshFilter>();
//	platform12Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform12Trigger->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 214, 350, 0});
//	platform12Trigger->GetTransform()->SetScale({ 23, 3, 10});
//	platform12Trigger->GetTransform()->SetEulerAngles({ 0, 0, -10});
//	platform12Trigger->AddComponent<BoxCollider2D>();
//	platform12Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	GameObject* platform13 = CreateGameObject();
//	platform13->AddComponent<MeshRenderer>();
//	platform13->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform13->AddComponent<MeshFilter>();
//	platform13->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform13->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 300, 410, 0});
//	platform13->GetTransform()->SetScale({ 25, 20, 10});
//	platform13->GetTransform()->SetEulerAngles({ 0, 0, 10});
//	platform13->AddComponent<BoxCollider2D>();
//
//	GameObject* platform13Trigger = CreateGameObject();
//	platform13Trigger->AddComponent<MeshRenderer>();
//	platform13Trigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	platform13Trigger->AddComponent<MeshFilter>();
//	platform13Trigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	platform13Trigger->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 300, 420, 0});
//	platform13Trigger->GetTransform()->SetScale({ 23, 3, 10});
//	platform13Trigger->GetTransform()->SetEulerAngles({ 0, 0, 10});
//	platform13Trigger->AddComponent<BoxCollider2D>();
//	platform13Trigger->GetComponent<BoxCollider2D>()->isTrigger = true;
//
//	// Left side now
//
//	GameObject* boostPlatform = CreateGameObject();
//	boostPlatform->AddComponent<MeshRenderer>();
//	boostPlatform->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	boostPlatform->AddComponent<MeshFilter>();
//	boostPlatform->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	boostPlatform->GetTransform()->SetPosition({ 100, 410, 0});
//	boostPlatform->GetTransform()->SetScale({ 40, 10, 10});
//	boostPlatform->GetTransform()->SetEulerAngles({ 0, 0, 10});
//	boostPlatform->AddComponent<BoxCollider2D>();
//
//	GameObject* boostPlatformTrigger = CreateGameObject();
//	boostPlatformTrigger->AddComponent<MeshRenderer>();
//	boostPlatformTrigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	boostPlatformTrigger->AddComponent<MeshFilter>();
//	boostPlatformTrigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	boostPlatformTrigger->GetTransform()->SetPosition({ 100, 417, 0});
//	boostPlatformTrigger->GetTransform()->SetScale({ 38, 3, 10});
//	boostPlatformTrigger->GetTransform()->SetEulerAngles({ 0, 0, 10});
//	boostPlatformTrigger->AddComponent<BoxCollider2D>();
//	//boostPlatformTrigger->AddComponent<BoostPlatform>();
//	//auto boost = boostPlatformTrigger->GetComponent<BoostPlatform>();
//	//boost->SetForce({ 30, 0, 0 });
//	//boost->SetPlatformBase(boostPlatform->GetComponent<BoxCollider2D>());
//
//	GameObject* boostPlatformSupport = CreateGameObject();
//	boostPlatformSupport->AddComponent<MeshRenderer>();
//	boostPlatformSupport->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	boostPlatformSupport->AddComponent<MeshFilter>();
//	boostPlatformSupport->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	boostPlatformSupport->GetTransform()->SetPosition({ m_worldWidth * 0.5f, 420, 0});
//	boostPlatformSupport->GetTransform()->SetScale({ 50, 10, 10});
//	boostPlatformSupport->GetTransform()->SetEulerAngles({ 0, 0, 20});
//	boostPlatformSupport->AddComponent<BoxCollider2D>();
//
//	GameObject* triangularSplit = CreateGameObject();
//	triangularSplit->AddComponent<MeshRenderer>();
//	triangularSplit->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	triangularSplit->AddComponent<MeshFilter>();
//	triangularSplit->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_TRIANGLE]);
//	triangularSplit->GetTransform()->SetPosition({ 400, 150, 0});
//	triangularSplit->GetTransform()->SetScale({ 50, 35, 10});
//	triangularSplit->GetTransform()->SetEulerAngles({ 0, 0, 180});
//	triangularSplit->AddComponent<PolygonCollider2D>();
//	triangularSplit->GetComponent<PolygonCollider2D>()->SetToTriangleCollider();
//
//	GameObject* triangleBoostTrigger = CreateGameObject();
//	triangleBoostTrigger->AddComponent<MeshRenderer>();
//	triangleBoostTrigger->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	triangleBoostTrigger->AddComponent<MeshFilter>();
//	triangleBoostTrigger->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	triangleBoostTrigger->GetTransform()->SetPosition({ 400, 168, 0});
//	triangleBoostTrigger->GetTransform()->SetScale({ 42, 3, 10});
//	triangleBoostTrigger->GetTransform()->SetEulerAngles({ 0, 0, 0});
//	triangleBoostTrigger->AddComponent<BoxCollider2D>();
//	//triangleBoostTrigger->AddComponent<BoostPlatform>();
//	//auto triangle_boost = triangleBoostTrigger->GetComponent < BoostPlatform >();
//	//triangle_boost->SetForce({ -30, 0, 0 });
//	//triangle_boost->SetPlatformBase(triangularSplit->GetComponent<PolygonCollider2D>());
//
//
//	GameObject* finish = CreateGameObject();
//	finish->AddComponent<MeshRenderer>();
//	finish->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	finish->AddComponent<MeshFilter>();
//	finish->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_CUBE]);
//	finish->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 170, 460, 0});
//	finish->GetTransform()->SetScale({ 50, 15, 10});
//	finish->AddComponent<BoxCollider2D>();
//
//	GameObject* finishCircle= CreateGameObject();
//	finishCircle->AddComponent<MeshRenderer>();
//	finishCircle->GetComponent<MeshRenderer>()->SetMaterial(newMaterial);
//	finishCircle->AddComponent<MeshFilter>();
//	finishCircle->GetComponent<MeshFilter>()->SetMesh(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEO_BALL]);
//	finishCircle->GetTransform()->SetPosition({ m_worldWidth * 0.5f + 170, 476.5f, 0});
//	finishCircle->GetTransform()->SetScale({ 7.5f, 7.5f, 10});
//	finishCircle->AddComponent<CircleCollider2D>();
	//finishCircle->AddComponent<FinishPoint>();
	//auto fp = finishCircle->GetComponent<FinishPoint>();
	//fp->SetPlayer(player->GetComponent<CircleCollider2D>());

	//_fp = fp;



#pragma endregion Platforms

#pragma endregion GameObject_Setup
	for (const auto& i : gameObjects_)
	{
		if (i->ActiveSelf())
		{
			i->Update(0.01);
		}
	}

	for (auto& i : drops_)
	{
		i->GetGameObject()->SetActive(false);
	}
	drops_[0]->GetGameObject()->SetActive(true);
	

}

void Scene1::Update(double dt)
{
	// Check for key press, you can add more for interaction
	HandleKeyPress();
	static KeyboardController* kc = KeyboardController::GetInstance();
	//for (auto& i : _drops)
	//{
	//	if (!i->IsDropping())
	//	{
	//		i->GetGameObject()->SetActive(false);
	//		_dropQueue.emplace_back(i);
	//	}
	//}


	if (kc->IsKeyDown('Z'))
	{
		static int i = 0;
		if (!drops_[i]->IsDropping())
		{
			drops_[i]->GetGameObject()->SetActive(false);
			i = ++i % drops_.size();
			drops_[i]->GetGameObject()->SetActive(true);
		}
	}

	


	timePassed_ += dt;

	if (timePassed_ >= FIXED_UPDATE_FRAME_TIME)
	{
		for (const auto& i : gameObjects_)
		{
			if (i->ActiveSelf())
			{
				i->BeforeFixedUpdate(dt);
			}
		}

		static ColliderManager* cm = ColliderManager::GetInstance();
		cm->Update(dt);

		for (const auto& i : gameObjects_)
		{
			if (i->ActiveSelf())
			{
				i->FixedUpdate(dt);
			}
		}
		timePassed_ -= FIXED_UPDATE_FRAME_TIME;
	}


	for (const auto& i : gameObjects_)
	{
		if (i->ActiveSelf())
		{
			i->Update(dt);
		}
	}
}

void Scene1::Render()
{
	// Clear color buffer every frame
	SceneRenderer::GetInstance()->Clear();

	constexpr glm::vec4 clear_colour{ 189.f / 255.f, 212.f / 255.f, 230.f / 255.f, 0.0f };
	SceneRenderer::GetInstance()->SetClearColour(clear_colour);

	SceneRenderer::GetInstance()->BeginScene(sceneCamera_, shaderLib_);

	for (const auto& i : gameObjects_)
	{
		if (i->ActiveSelf())
		{
			i->Render();
		}
	}

	//if (_fp->IsGameEnded())
	//	SceneRenderer::GetInstance()->RenderText(SceneRenderer::GetInstance()->GetMeshList()[SceneRenderer::GEOMETRY_TYPE::GEO_TEXT], "You win!", Color(0, 0, 0), 3, 42, 34);

	SceneRenderer::GetInstance()->EndScene();

}

//void Scene1::RenderMesh(Mesh* mesh, bool enableLight)
//{
//	glm::mat4 MVP;
//
//	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
//	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
//	mesh->Render();
//}


void Scene1::Exit()
{
	delete yellowMat_;
	delete shaderLib_;
	for (auto& i : gameObjects_)
		if (i)
			delete i;

	for (auto& i : toCleanUp_)
		delete i;
}

GameObject* Scene1::CreateGameObject()
{
	GameObject* temp = new GameObject();
	gameObjects_.push_back(temp);
	return temp;
}

void Scene1::HandleKeyPress() 
{
	if (Application::IsKeyPressed(0x31))
	{
		// Key press to enable culling
		glEnable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed(0x32))
	{
		// Key press to disable culling
		glDisable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed(0x33))
	{
		// Key press to enable fill mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	}
	if (Application::IsKeyPressed(0x34))
	{
		// Key press to enable wireframe mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	}

}

