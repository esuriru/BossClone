#pragma once

#include "Core/Window.h"
#include <glm/glm.hpp>
#include "Events/Event.h"
#include "Core/Core.h"
#include <functional>

using EventCallback = std::function<void(Event&)>;

struct WindowProps
{
	std::string Title;
	unsigned int Width;
	unsigned int Height;

	/// <summary>
	/// Constructor for window properties.
	/// </summary>
	/// <param name="title">: Title for window</param>
	/// <param name="width">: Window width</param>
	/// <param name="height">: Window height</param>
	WindowProps(
		std::string title = "Boss Clone",
		unsigned int width = 1280,
		unsigned int height = 720
		)
		: Title(std::move(title)), Width(width), Height(height)
	{}
};

struct GLFWwindow;

class Window
{
public:

	explicit Window(const WindowProps& props = WindowProps());
	virtual ~Window();

	void OnUpdate();

	void SetVSyncEnabled(bool enabled);
	bool IsVSyncEnabled() const;

	void SetCursorEnabled(bool enabled);
	bool IsCursorEnabled() const;

#pragma region INLINE_FUNCTIONS
	inline unsigned int GetWidth() const
	{
		return data_.Width;
	}
	
	inline unsigned int GetHeight() const 
	{
		return data_.Height;
	}

	inline glm::vec2 GetSize() const
	{
		return { GetWidth(), GetHeight() };
	}

	inline void SetEventCallback(const EventCallback& cb) 
	{
		data_.EventCallback = cb;
	}

	inline virtual void* GetNativeWindow() const 
	{
		return window_;
	}

#pragma endregion INLINE_FUNCTIONS

private:
#pragma region PRIVATE_FUNCTIONS
	virtual void Init(const WindowProps& props);
	virtual void Exit();
#pragma endregion PRIVATE_FUNCTIONS

#pragma region PRIVATE_DATA_MEMBERS
	GLFWwindow* window_;

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height;
		bool VSyncEnabled = false;

		EventCallback EventCallback;
	};

	WindowData data_;
	bool cursorEnabled_;
#pragma endregion PRIVATE_DATA_MEMBERS
};
