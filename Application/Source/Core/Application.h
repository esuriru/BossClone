
#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.h"
#include <memory>
#include <Scene/Scene.h>
#include <Scene/SceneLayer.h>

#include "LayerStack.h"
#include "Singleton.h"
#include "timer.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Scene/GameObject.h"

class LoadingScene;
struct GLFWwindow;

class Application : public Singleton<Application>
{
public:
	Application();
	~Application();

	void Init();
	void Run();
	void Exit();

	// Use this
	void QueueSceneLoad(const std::string& name);

	void LoadScene(const std::string& name);

	void OnEvent(Event& e);

	inline Window& GetWindow()
	{
		return *window_;
	}

	static bool IsKeyPressed(unsigned short key);

	void UpdateLoadingScene(std::string text = "");
	void PerformRunCycle();

#pragma region EVENTS

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

	int GetWindowWidth() const;
	int GetWindowHeight() const;

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	void AttachScene(SceneLayer* scene);
	void DetachScene(SceneLayer* scene);

	inline Scene& GetActiveScene() const
	{
		return *activeScene_;
	}

	std::shared_ptr<GameObject> CreateGameObject();

	bool loading;
private:

	StopWatch timer_;
	LayerStack layerStack_;

	bool running_;
	bool minimized_;
	bool windowCursor_;
	
	SceneLayer* sceneLayer_;
	std::shared_ptr<Scene> activeScene_;
	std::shared_ptr<Window> window_;
	std::shared_ptr<LoadingScene> loadingScene_;

};

#endif