
#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.h"
#include <memory>
#include <Scene/Scene.h>

#include "LayerStack.h"
#include "Singleton.h"
#include "timer.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include "ImGui/ImGuiLayer.h"

#include "Utils/Singleton.h"
#include "Renderer/OrthographicCamera.h"

#include "Game/MainLayer.h"

struct GLFWwindow;

class Application : public Utility::Singleton<Application>
{
public:
	Application();
	~Application();

	auto Run() -> void;

	auto OnEvent(Event& e) -> void;

	inline auto GetWindow() -> Window&
	{
		return *window_;
	}

	auto PerformRunCycle() -> void;

#pragma region EVENTS

    auto OnGameStateChange(GameStateChangeEvent& e) -> bool;

	// Window Events

	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);

	// End Window Events

	// Mouse Events

	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	bool OnMouseMove(MouseMovedEvent& e);
	bool OnScroll(MouseScrolledEvent& e);

	// End Mouse Events

	// Keyboard Events

	bool OnKeyPress(KeyPressedEvent& e);
	bool OnKeyRelease(KeyReleasedEvent& e);
	
	// End Keyboard Events

#pragma endregion EVENTS

	inline auto GetWindowWidth() const -> uint32_t 
    {
        return window_->GetWidth();
    }

	inline auto GetWindowHeight() const -> uint32_t 
    {
        return window_->GetHeight();
    }

	auto PushLayer(Layer* layer) -> void;
	auto PushOverlay(Layer* overlay) -> void;

private:
    OrthographicCamera camera_;

	StopWatch timer_;
	LayerStack layerStack_;

	bool running_;
	bool minimized_;
	bool windowCursor_;
	
	std::shared_ptr<Window> window_;

    ImGuiLayer* imGuiLayer_;
    Layer* mainLayer_;
    Layer* menuLayer_;

    bool changingToPlay_ = false;
    bool changingToMenu_ = false;
};

#endif