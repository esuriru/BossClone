#include "Application.h"

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
#include "ECS/Component.h"
#include "ECS/Coordinator.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"
#include "Game/GameManager.h"

#include "Scene/SceneManager.h"
#include <chrono>
#include "Utils/Input.h"
#include "Timestep.h"
#include "Layer.h"
#include "Events/EventDispatcher.h"
#include "Scene/Scene.h"
#include "Game/MenuLayer.h"

#include "ImGui/ImGuiLayer.h"

#include "Utils/SoundController.h"
#include "Utils/MusicPlayer.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>


auto Application::OnEvent(Event& e) -> void
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

	dispatcher.Dispatch<GameStateChangeEvent>(CC_BIND_EVENT_FUNC(Application::OnGameStateChange));

	for (auto it = layerStack_.end(); it != layerStack_.begin();)
	{
		if (e.Handled)
			break;
		(*--it)->OnEvent(e);
	}
}

auto Application::PerformRunCycle() -> void
{
	static Input* input = Input::Instance();
	if (!minimized_)
	{
		const Timestep deltaTime = Timestep(static_cast<float>(timer_.getElapsedTime()));

        static MusicPlayer* musicPlayer = MusicPlayer::Instance();
        musicPlayer->UpdateTransition(deltaTime);

		for (const auto& layer : layerStack_)
        {
			layer->OnUpdate(deltaTime);
        }

        imGuiLayer_->Begin();
        for (const auto& layer : layerStack_)
        {
            layer->OnImGuiRender();
        }
        imGuiLayer_->End(); 
	}
    
    if (changingToPlay_)
    {
        changingToPlay_ = false;
        layerStack_.PopLayer(menuLayer_);
        PushLayer(mainLayer_);
    }

    if (changingToMenu_)
    {
        changingToMenu_ = false;
        layerStack_.PopLayer(mainLayer_);
        PushLayer(menuLayer_);
    }

	input->PostUpdateKeys();
	window_->OnUpdate();
}

auto Application::OnGameStateChange(GameStateChangeEvent &e) -> bool
{
    switch (e.GetNewState())
    {
    case GameState::PlayingLevel: 
        changingToPlay_ = true;
        break;
    case GameState::MenuLevel:
        changingToMenu_ = true;
        break;
    default:
        break;
    }
    return true;
}

auto Application::OnWindowClose(WindowCloseEvent& e) -> bool 
{
	// Turn off the application.
	running_ = false;
	return false;
}

auto Application::OnWindowResize(WindowResizeEvent& e) -> bool 
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		minimized_ = true;
		return false;
	}

	minimized_ = false;

	// Maybe this needs to be in the renderer?
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
#if 0
	static SceneRenderer* sr = SceneRenderer::Instance();
	sr->OnWindowResize(e.GetWidth(), e.GetHeight());
#endif
	return false;
}

static Input* input = Input::Instance();
auto Application::OnMouseButtonPressed(MouseButtonPressedEvent& e) -> bool
{
	input->UpdateMouseButtonMap(e.GetMouseButton(), GLFW_PRESS);
	return false;
}

auto Application::OnMouseButtonReleased(MouseButtonReleasedEvent& e) -> bool
{
	input->UpdateMouseButtonMap(e.GetMouseButton(), GLFW_RELEASE);
	return false;
}

auto Application::OnMouseMove(MouseMovedEvent& e) -> bool
{
	input->UpdateMouseCoords(e.GetCoords());
	return false;
}

auto Application::OnScroll(MouseScrolledEvent& e) -> bool
{
	input->UpdateScrollOffset(e.GetOffset());
	return false;
}

auto Application::OnKeyPress(KeyPressedEvent& e) -> bool
{
	input->UpdateKeys(e.GetKeyCode(), GLFW_PRESS);
#if 0
    if (e.GetKeyCode() == GLFW_KEY_LEFT_ALT)
    {
        if (!windowCursor_)
        {
            windowCursor_ = true;
            window_->SetCursorEnabled(true);
        }
    }
#endif

    return false;
}

auto Application::OnKeyRelease(KeyReleasedEvent& e) -> bool
{
	input->UpdateKeys(e.GetKeyCode(), GLFW_RELEASE);
#if 0
	if (e.GetKeyCode() == GLFW_KEY_LEFT_ALT)
	{
		if (windowCursor_)
		{
			windowCursor_ = false;
			window_->SetCursorEnabled(false);
		}
	}
#endif

	return false;
}

auto Application::PushLayer(Layer* layer) -> void
{
	layerStack_.PushLayer(layer);
}

auto Application::PushOverlay(Layer* overlay) -> void
{
	layerStack_.PushOverlay(overlay);
}

Application::Application()
    : camera_(-1.6f, 1.6f, -0.9f, 0.9f)
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
    window_->SetCursorEnabled(true);

    SoundController::Instance()->Init();
    MusicPlayer::Instance()->Init();
    for (int i = 0; i < 9; ++i)
        MusicPlayer::Instance()->MasterVolumeDecrease();

    RenderCommand::Init();
    Renderer2D::Init();

    mainLayer_ = new MainLayer();
    menuLayer_ = new MenuLayer();

    PushLayer(menuLayer_);

    imGuiLayer_ = new ImGuiLayer();
    PushOverlay(imGuiLayer_);

    GameManager::Instance()->SetEventCallback(CC_BIND_EVENT_FUNC(Application::OnEvent));

	running_ = true;
}

Application::~Application()
{
	//Finalize and clean up GLFW
	glfwTerminate();

    Input::Destroy();
    Coordinator::Destroy();
}

auto Application::Run() -> void
{
	timer_.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (running_)
	{
		PerformRunCycle();
	} 
}

