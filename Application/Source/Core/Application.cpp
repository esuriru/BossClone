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
        if (e.GetOldState() == GameState::MenuLevel )
            changingToPlay_ = true;
        break;
    case GameState::MenuLevel:
        if (e.GetOldState() == GameState::PlayingLevel || e.GetOldState() == GameState::Win)
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

    {
        SoundInfo* source;
        MusicPlayer::Instance()->AddMusic("Assets/Sounds/level1bgm_01.ogg", 1, source, true, true);
        MusicPlayer::Instance()->SetIDToName(1, "My Castle Town");
        source->SetVolume(0.0f);
    }

    {
        SoundInfo* source;
        MusicPlayer::Instance()->AddMusic("Assets/Sounds/menu_01.ogg", 2, source, true, true);
        source->SetVolume(0.0f);
    }

    {
        SoundInfo* source;
        MusicPlayer::Instance()->AddMusic("Assets/Sounds/01.-The-Tale-of-a-Cruel-World.ogg", 3, source, true, true);
        MusicPlayer::Instance()->SetIDToName(3, "The Tale of a Cruel World");
        source->SetVolume(0.0f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/FreeSFX/GameSFX/Events/Retro Event UI 01.wav", 1, true);
        source->SetVolume(0.5f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/03_Step_grass_03.wav", 2, true, false);
        source->SetVolume(0.5f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/30_Jump_03.wav", 3, true, false);
        source->SetVolume(0.9f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/45_Landing_01.wav", 4, true, false);
        source->SetVolume(0.9f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/22_Slash_04.wav", 5, true, false);
        source->SetVolume(0.2f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/04_Fire_explosion_04_medium.wav", 6, true, false);
        source->SetVolume(0.8f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/woodhit.ogg", 7, true, false, SoundInfo::SOUNDTYPE::_3D);
        source->SetVolume(0.95f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/woodbreak.ogg", 8, true, false, SoundInfo::SOUNDTYPE::_3D);
        source->SetVolume(0.95f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/013_Confirm_03.wav", 9, true, false);
        source->SetVolume(0.95f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/39_Block_03.wav", 10, true, false);
        source->SetVolume(0.95f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/08_Bite_04.wav", 11, true, false);
        source->SetVolume(0.85f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/02_Heal_02.wav", 12, true, false);
        source->SetVolume(0.65f);
    }

    {
        SoundInfo* source;
        SoundController::Instance()->LoadSound(source, "Assets/Sounds/88_Teleport_02.wav", 13, true, false);
        source->SetVolume(0.85f);
    }

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

