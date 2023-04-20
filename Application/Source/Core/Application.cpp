#include "Application.h"

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include "Scene/LoadingScene.h"
#include "Physics/ColliderManager.h"
#include "Scene/SceneManager.h"
#include <chrono>
#include "Utils/Input.h"
#include "Timestep.h"
#include "Layer.h"
#include "Events/EventDispatcher.h"
#include "Scene/SceneLayer.h"
#include "Scene/DebugScene.h"
#include "Scene/LoadingScene.h"
#include "Scene/ArcadeScene.h"
#include "Scene/MainMenuScene.h"
#include "Scene/Scene.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

//Include GLEW
//#include <GL/glew.h>

//Include GLFW

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>
#include "Scene/BottleThrowScene.h"
#include "Scene/JungleScene.h"
#include "Scene/MainScene.h"
#include "Scene/BumperCarScene.h"
#include "Scene/ArcadeScene.h"
#include "Scene/PoolTableScene.h"
//#include "KeyboardController.h"
#include "Utils/Input.h"
#include "Renderer/SceneRenderer.h"

void Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);

	// Bind Window callbacks
	dispatcher.Dispatch<WindowCloseEvent>(CC_BIND_EVENT_FUNC(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(CC_BIND_EVENT_FUNC(Application::OnWindowResize));

	// Bind Mouse callbacks
	dispatcher.Dispatch<MouseButtonPressedEvent>(CC_BIND_EVENT_FUNC(Application::OnMouseButtonPressed));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(CC_BIND_EVENT_FUNC(Application::OnMouseButtonReleased));
	dispatcher.Dispatch<MouseMovedEvent>(CC_BIND_EVENT_FUNC(Application::OnMouseMove));
	dispatcher.Dispatch<MouseScrolledEvent>(CC_BIND_EVENT_FUNC(Application::OnScroll));

	// Bind Keyboard callbacks
	dispatcher.Dispatch<KeyPressedEvent>(CC_BIND_EVENT_FUNC(Application::OnKeyPress));
	dispatcher.Dispatch<KeyReleasedEvent>(CC_BIND_EVENT_FUNC(Application::OnKeyRelease));

	for (auto it = layerStack_.end(); it != layerStack_.begin();)
	{
		if (e.Handled)
			break;
		(*--it)->OnEvent(e);
	}
}


bool Application::IsKeyPressed(unsigned short key)
{
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

void Application::UpdateLoadingScene(std::string text)
{
	loadingScene_->UpdateText(text);
	PerformRunCycle();
}

void Application::PerformRunCycle()
{
	static Input* input = Input::GetInstance();
	if (!minimized_)
	{
		const Timestep deltaTime = Timestep(timer_.getElapsedTime());

		for (const auto& layer : layerStack_)
			layer->OnUpdate(deltaTime);
	}

	input->PostUpdateKeys();
	window_->OnUpdate();
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
	// Turn off the application.
	running_ = false;
	return false;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		minimized_ = true;
		return false;
	}

	minimized_ = false;

	// Maybe this needs to be in the renderer?
	static SceneRenderer* sr = SceneRenderer::GetInstance();
	sr->OnWindowResize(e.GetWidth(), e.GetHeight());
	return true;
}

static Input* input = Input::GetInstance();
bool Application::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	input->UpdateMouseButtonMap(e.GetMouseButton(), GLFW_PRESS);
	return true;
}

bool Application::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	input->UpdateMouseButtonMap(e.GetMouseButton(), GLFW_RELEASE);
	return true;
}

bool Application::OnMouseMove(MouseMovedEvent& e)
{
	input->UpdateMouseCoords(e.GetCoords());
	return true;
}

bool Application::OnScroll(MouseScrolledEvent& e)
{
	input->UpdateScrollOffset(e.GetOffset());
	return true;
}

bool Application::OnKeyPress(KeyPressedEvent& e)
{
	input->UpdateKeys(e.GetKeyCode(), GLFW_PRESS);
	if (e.GetKeyCode() == GLFW_KEY_LEFT_ALT)
	{
		if (!windowCursor_)
		{
			windowCursor_ = true;
			window_->SetCursorEnabled(true);
		}
	}

	return true;
}

bool Application::OnKeyRelease(KeyReleasedEvent& e)
{
	input->UpdateKeys(e.GetKeyCode(), GLFW_RELEASE);
	if (e.GetKeyCode() == GLFW_KEY_LEFT_ALT)
	{
		if (windowCursor_)
		{
			windowCursor_ = false;
			window_->SetCursorEnabled(false);
		}
	}

	return true;
}

int Application::GetWindowWidth() const
{
	return window_->GetWidth();
}

int Application::GetWindowHeight() const
{
	return window_->GetHeight();
}

void Application::PushLayer(Layer* layer)
{
	layerStack_.PushLayer(layer);
}

void Application::PushOverlay(Layer* overlay)
{
	layerStack_.PushOverlay(overlay);
}

void Application::AttachScene(SceneLayer* scene)
{
	PushLayer(scene);
	activeScene_ = scene->scene;
}

void Application::DetachScene(SceneLayer* scene)
{
	layerStack_.PopLayer(scene);
	activeScene_ = nullptr;
}

std::shared_ptr<GameObject> Application::CreateGameObject()
{
	return activeScene_ ? activeScene_->CreateGameObject() : nullptr;
}

Application::Application()
{
}

Application::~Application()
{
}


void mouse_press_callback(GLFWwindow* window, int button, int action, int mods)
{
	input->UpdateMouseButtonMap(button, action);
}

static std::chrono::steady_clock::time_point begin;
static std::chrono::steady_clock::time_point end;

void Application::Init()
{
	//Initialize GLFW
	CC_ASSERT(glfwInit(), "GLFW could not be initialized.")

	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); //Request a specific OpenGL version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

	window_ = std::make_shared<Window>();
	window_->SetEventCallback(CC_BIND_EVENT_FUNC(Application::OnEvent));
	window_->SetVSyncEnabled(false);

	Shader::BillboardShader = ShaderLibrary::GetInstance()->Load("Billboard", "Shader/billboard.vertexshader", "Shader/billboard.fragmentshader");
	ShaderLibrary::GetInstance()->Load("Environment Map", "Shader/envmap.vertexshader", "Shader/envmap.fragmentshader");
	//minimized_ = false;
	ShaderLibrary::GetInstance()->Load("skybox proprietary shader", "Shader/skybox.vertexshader", "Shader/skybox.fragmentshader");
	ShaderLibrary::GetInstance()->Load("Terrain Shader", "Shader/basic.vertexshader", "Shader/multitexture.fragmentshader");
	ShaderLibrary::GetInstance()->Load("Text", "Shader/Experimental/IllumZero.vertexshader", "Shader/Experimental/Text.fragmentshader");
	ShaderLibrary::illumModelShaders_ =
	{
		ShaderLibrary::GetInstance()->Load("Illum 0", "Shader/Experimental/IllumZero.vertexshader", "Shader/Experimental/IllumZero.fragmentshader"),
		ShaderLibrary::GetInstance()->Load("Illum 1", "Shader/Experimental/IllumOne.vertexshader", "Shader/Experimental/IllumOne.fragmentshader"),
		ShaderLibrary::GetInstance()->Load("Experimental Blinn Phong", "Shader/Experimental/BlinnPhong.vertexshader", "Shader/Experimental/BlinnPhong.fragmentshader")
	};

	Shader::DefaultShader = ShaderLibrary::illumModelShaders_[2];
	Shader::DefaultShader->Bind();

	ShaderLibrary::GetInstance()->RegisterUniformBuffer("Material");
	ShaderLibrary::GetInstance()->RegisterUniformBuffer("LightBlock");

	//PushLayer(scene_layer);
	loadingScene_ = SceneManager::GetInstance()->CreateScene<LoadingScene>("LoadingScene");

	sceneLayer_ = new SceneLayer();
	PushLayer(sceneLayer_);
	sceneLayer_->scene = nullptr; 
	SceneManager::GetInstance()->CreateScene<MainScene>("MainScene");
	SceneManager::GetInstance()->CreateScene<DebugScene>("DebugScene");
	SceneManager::GetInstance()->CreateScene<JungleScene>("JungleScene");
	SceneManager::GetInstance()->CreateScene<ArcadeScene>("ArcadeScene");
	SceneManager::GetInstance()->CreateScene<BumperCarScene>("BumperCarScene");
	SceneManager::GetInstance()->CreateScene<BottleThrowScene>("BottleThrowScene");
	SceneManager::GetInstance()->CreateScene<MainMenuScene>("MainMenuScene");
	//SceneManager::GetInstance()->CreateScene<ArcadeScene>("ArcadeScene");
	LoadScene("MainMenuScene");
	//LoadScene("BottleThrowScene");
	//LoadScene("DebugScene");


	running_ = true;
}

void Application::Run()
{
	//Main Loop
	timer_.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (running_)
	{
		PerformRunCycle();
	} 
}


void Application::Exit()
{
	SceneRenderer::Destroy();
	Input::Destroy();
	ShaderLibrary::Destroy();
	ColliderManager::Destroy();
	SceneManager::Destroy();

	//Finalize and clean up GLFW
	glfwTerminate();
}

void Application::QueueSceneLoad(const std::string& name)
{
	Application::activeScene_->sceneNameToChangeTo_ = name;
	Application::activeScene_->isChangingScene_ = true;
	loading = true;
}

void Application::LoadScene(const std::string& name)
{
	auto scene_to_load = SceneManager::GetInstance()->GetScene(name);
	if (!scene_to_load)
	{
		CC_WARN("Could not load a null scene.");
		return;
	}
	std::shared_ptr<Scene> previousScene = activeScene_;
	activeScene_ = loadingScene_;
	sceneLayer_->scene = activeScene_;

	SceneRenderer::GetInstance()->Reset();
		
	loadingScene_->BindScene(scene_to_load);
	//SceneRenderer::GetInstance()->Clear();
	//SceneRenderer::GetInstance()->SetClearColour({0.6f, 0.6f, 0.6f, 1});

	loadingScene_->Init();
	PerformRunCycle();
	CC_INFO("Starting to load '", name, "' scene.");
	if (previousScene)
		previousScene->Exit();
	ColliderManager::GetInstance()->ClearColliders();
	loadingScene_->ChangeScene();

	CC_INFO("Scene '", name, "' loaded.");
	//loading = true;
	loadingScene_->Exit();
	activeScene_ = scene_to_load;
	sceneLayer_->scene = activeScene_;
	timer_.getElapsedTime();
	loading = false;
	activeScene_->isChangingScene_ = false;
	//_CrtDumpMemoryLeaks();
}
