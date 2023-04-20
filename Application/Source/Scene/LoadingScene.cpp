#include "LoadingScene.h"
#include "Core/Application.h"
#include "GameObject.h"
#include "Renderer/SceneRenderer.h"
#include "Renderer/MeshBuilder.h"
#include "MyMath.h"

#include "Components/UIRenderer.h"
#include "Components/MeshFilter.h"
#include "Components/Text.h"
#include "Utils/Font.h"

#include "Utils/Input.h"

LoadingScene::LoadingScene()
{
	backgroundMesh_ = MeshBuilder::GenerateQuad("Background", glm::vec4(1, 0, 1, 1));
	backgroundTexture_ = Texture::LoadTexture("Assets/vex.jpg", false);
	backgroundMat_ = new Material();
	backgroundMat_->textureMaps[Material::DIFFUSE] = backgroundTexture_;
	backgroundMat_->materialShader = ShaderLibrary::GetShaderFromIllumModel(0);
	inconsolataFont_ = Font::LoadFont("Assets/fonts/inconsolata.tga", "Assets/fonts/inconsolata.csv");
	inconsolataFont_->fontMultiplier = 0.014f;
	backgroundMesh_->materials.push_back(backgroundMat_);

	quadMesh_ = MeshBuilder::GenerateQuad("Progress bar", glm::vec4(1, 0, 1, 1));
	progressBarMat_ = new Material();
	progressBarMat_->data->_diffuse = glm::vec4(1, 0, 1, 1);
	progressBarMat_->materialShader = ShaderLibrary::GetShaderFromIllumModel(0);

	auto background_obj = CreateGameObject();
	background_obj->AddComponent<MeshFilter>()->SetMesh(backgroundMesh_);
	background_obj->AddComponent<UIRenderer>()->renderOrder = 0;
	background_obj->GetTransform()->SetScale({ Application::GetInstance()->GetWindowWidth(), Application::GetInstance()->GetWindowHeight(), 1 });
	background_obj->GetTransform()->SetPosition({ Application::GetInstance()->GetWindowWidth() * 0.5f, Application::GetInstance()->GetWindowHeight() * 0.5f, 1 });

	auto camera_object = CreateGameObject();
	sceneCamera_ = camera_object->AddComponent<Camera>();
	sceneCamera_->projection = glm::ortho(0.f, static_cast<float>(Application::GetInstance()->GetWindowWidth()),
		0.f, static_cast<float>(Application::GetInstance()->GetWindowHeight()), -10.f, 10.f);
	sceneCamera_->Init(
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	auto textObject = CreateGameObject();
	displayText_ = textObject->AddComponent<Text>();
	displayText_->SetText("Initializing scene.");
	displayText_->colour = { 1, 1, 1 };
	displayText_->onScreen = true;
	displayText_->font = inconsolataFont_;

	textObject->GetTransform()->SetPosition({ 0, 0, 0 });
	textObject->GetTransform()->SetScale({ 45, 52, 1 });

	auto progressBar = CreateGameObject();
	progressBar->AddComponent<MeshFilter>()->SetMesh(quadMesh_);
	progressBar->AddComponent<UIRenderer>()->SetMaterial(progressBarMat_);
	progressBarTransform_ = progressBar->GetTransform();
	progressBarTransform_->SetPosition({ Application::GetInstance()->GetWindowWidth() * 0.5f, 0, 0 });


}

LoadingScene::~LoadingScene()
{
	delete inconsolataFont_;
	delete quadMesh_;
	delete backgroundMesh_;
	delete backgroundMat_;
	delete backgroundTexture_;

	objects_.clear();
}

void LoadingScene::ChangeScene()
{
	isLoading_ = true;

	sceneToLoad_->Init();
}

void LoadingScene::Init()
{
	if (!sceneToLoad_)
	{
		CC_FATAL("Scene to load does not exist/is nullptr!");
		return;
	}
	Math::InitRNG();

	displayText_->SetText("Initializing scene.");
	inverseTimesLoad_ = sceneToLoad_->applicationRefreshes_ ? 1.f / sceneToLoad_->applicationRefreshes_ : 1.f;
	counter_ = 0.0f;
	progressBarTransform_->SetScale({ 0, 10, 1 });

	static SceneRenderer* renderer = SceneRenderer::GetInstance();
	renderer->Clear();
	renderer->SetClearColour({ 0.6f, 0.6f, 0.6f, 1 });
}

void LoadingScene::Update(double dt)
{
	for (const auto& i : objects_)
	{
		if (i->ActiveSelf())
		{
			i->Update(dt);
		}
	}

	if (isLoading_)
	{
		counter_ += 1.0f;
		progressBarTransform_->SetScale({ Application::GetInstance()->GetWindowWidth() * inverseTimesLoad_ * counter_, 10, 1 });
	}

	static Input* input = Input::GetInstance();
	input->Update();
}

void LoadingScene::Render()
{
	CC_TRACE("Loading Scene Render Call");
	static SceneRenderer* renderer = SceneRenderer::GetInstance();
	renderer->Clear();
	renderer->SetClearColour({ 0.6f, 0.6f, 0.6f, 1 });

	renderer->BeginScene(sceneCamera_, ShaderLibrary::GetInstance());

	for (const auto& i : objects_)
	{
		if (i->ActiveSelf())
			i->Render();
	}

	renderer->EndScene();
}

void LoadingScene::Exit()
{
	isLoading_ = false;

}

void LoadingScene::BindScene(std::shared_ptr<Scene> scene)
{
	sceneToLoad_ = scene;
}

void LoadingScene::UpdateText(std::string newMessage)
{
	displayText_->SetText(newMessage);
}
