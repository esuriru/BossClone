#include "MainMenuScene.h"
#include "GameObject.h"
#include "Renderer/SceneRenderer.h"
#include "MyMath.h"

#include "Components/UIRenderer.h"
#include "Components/MeshFilter.h"
#include "Renderer/MeshBuilder.h"
#include "Renderer/Texture.h"

#include "Utils/Input.h"
#include "Utils/Util.h"
#include "Core/Application.h"
#include "Components/Text.h"

MainMenuScene::MainMenuScene()
	: shaderLib_()
{
}

MainMenuScene::~MainMenuScene()
{
}

void MainMenuScene::Init()
{
	shaderLib_ = ShaderLibrary::GetInstance();
	auto envShader = shaderLib_->Get("Environment Map");
	envShader->SetFlags(ShaderFlag::ENVIRONMENT_MAPPED);

	Shader::MultitexturedShader = shaderLib_->Get("Terrain Shader");
	if (Shader::MultitexturedShader)
		Shader::MultitexturedShader->SetFlags(ShaderFlag::MULTI_TEXTURING);

	//Math::InitRNG();
	auto camera_object = CreateGameObject();
	sceneCamera_ = camera_object->AddComponent<Camera>();
	glm::mat4 projectionMatrix = glm::perspective(45.0f, static_cast<float>(Application::GetInstance()->GetWindowWidth()) / static_cast<float>(Application::GetInstance()->GetWindowHeight()), 0.1f, 1000.0f);
	sceneCamera_->projection = projectionMatrix;
	//sceneCamera_->projection = glm::ortho(0.f, worldWidth_m, 0.f, worldHeight_, -10.f, 10.f);
	sceneCamera_->Init(
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	//auto screen = CreateGameObject();
	//screen->AddComponent<UIRenderer>();
	//screen->AddComponent<MeshFilter>();
	mats_.push_back(new Material());
	meshList_[TITLE_MESH] = MeshBuilder::GenerateQuad("quad", { 1.f, 1.f, 1.f, 1.f }, 1000.f);
	meshList_[TITLE_MESH]->materials.push_back(mats_.back());
	meshList_[TITLE_MESH]->materials.front()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture("Assets/title.png");
	meshList_[TITLE_MESH]->materials.front()->materialShader = ShaderLibrary::GetShaderFromIllumModel(0);

	auto title_card = CreateGameObject();
	title_card->AddComponent<UIRenderer>();
	title_card->GetComponent<UIRenderer>()->renderOrder = 2;
	title_card->AddComponent<MeshFilter>()->SetMesh(meshList_[TITLE_MESH]);
	title_card->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() * 0.5f, Application::GetInstance()->GetWindowHeight() * 0.5f, 0 });
	title_card->GetTransform()->SetScale({ 1, 1, 1 });

	inconsolataFont_ = Font::LoadFont("Assets/fonts/inconsolata.tga", "Assets/fonts/inconsolata.csv");
	inconsolataFont_->fontMultiplier = 0.014f;

	std::shared_ptr<GameObject> text = CreateGameObject();
	startMessage_ = text->AddComponent<Text>();
	startMessage_->SetText("Press anywhere to start");
	text->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() * 0.5f - 200.f, Application::GetInstance()->GetWindowHeight() * 0.5f - 200.f, 0.f });
	//text->GetTransform()->SetPosition({ 0, 1240, 0 });
	text->GetTransform()->SetScale({ 40, 45, 1 });
	text->GetTransform()->SetEulerAngles({ 0, 0, 0 });
	startMessage_->font = inconsolataFont_;
	startMessage_->onScreen = true;

	skybox_ = new Skybox({
			"Assets/MainScene/GalaxyPanorama/px.png",
			"Assets/MainScene/GalaxyPanorama/nx.png",
			"Assets/MainScene/GalaxyPanorama/py.png",
			"Assets/MainScene/GalaxyPanorama/ny.png",
			"Assets/MainScene/GalaxyPanorama/pz.png",
			"Assets/MainScene/GalaxyPanorama/nz.png"
		});

	//if (!sceneToLoad_)
	//{
	//	CC_FATAL("Scene to load does not exist/is nullptr!");
	//	return;
	//}
	//sceneToLoad_->Init();
}

void MainMenuScene::Update(double dt)
{
	for (const auto& i : objects_)
	{
		if (i->ActiveSelf())
		{
			i->Update(dt);
		}
	}

	if (Input::GetInstance()->GetMouseButtonDown(0))
	{
		Application::GetInstance()->QueueSceneLoad("MainScene");
		return;
	}

	static Input* input = Input::GetInstance();
	input->Update();

	static float rotation = 0;
	rotation += 0.5f * dt;
	if (rotation > 360)
		rotation -= 360.f;


	constexpr glm::vec3 rotation_direction = glm::vec3(Util::sqrt(2), Util::sqrt(2), 0);
	skybox_->SetRotation(glm::rotate(glm::mat4(1.0f), glm::radians(rotation), rotation_direction));
}

void MainMenuScene::Render()
{
	if (isChangingScene_)
	{
		Application::GetInstance()->LoadScene(sceneNameToChangeTo_);
		isChangingScene_ = false;
		return;
	}
	static SceneRenderer* renderer = SceneRenderer::GetInstance();
	renderer->Clear();
	renderer->SetClearColour({ 1.f, 1.f, 1.f , 1.0f});

	renderer->BeginScene(sceneCamera_, ShaderLibrary::GetInstance(), skybox_, lights_, 0);

	for (const auto& i : objects_)
	{
		if (i->ActiveSelf())
			i->Render();
	}

	renderer->EndScene();
}

void MainMenuScene::Exit()
{
	delete inconsolataFont_;
	inconsolataFont_ = nullptr;
	
	if (skybox_)
	{
		delete skybox_;
		skybox_ = nullptr;
	}

	CC_TRACE("Deleting mesh list.");
	for (auto& i : meshList_)
	{
		delete i;
		i = nullptr;
	}

	CC_TRACE("Deleting material list.");
	for (auto& i : mats_)
	{
		delete i;
		i = nullptr;
	}
}

void MainMenuScene::BindScene(std::shared_ptr<Scene> scene)
{
	sceneToLoad_ = scene;
}
