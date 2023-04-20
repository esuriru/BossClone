#include "Window.h"


#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Application.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Core/Log.h"

static uint8_t s_GLFWWindowCount = 0;

static void GLFWErrorCallback(int error, const char* desc)
{
	CC_ERROR("GLFW Error: (", error, "): {", desc, "}");
}

Window::Window(const WindowProps& props)
{
	Window::Init(props);
}

Window::~Window()
{
	Window::Exit();
}

void Window::Init(const WindowProps& props)
{
	// Initialize data
	data_.Title = props.Title;
	data_.Width = props.Width;
	data_.Height = props.Height;

	CC_INFO("Creating window ", props.Title, "(", props.Width, ", ", props.Height, ")");
	//glewExperimental = true; // Needed for core profile


	if (s_GLFWWindowCount == 0)
	{
		const int glfw_init_success = glfwInit();

		// TODO: glfwTerminate on system shutdown
		CC_ASSERT(glfw_init_success, "Could not initialize GLFW.")
		glfwSetErrorCallback(GLFWErrorCallback);
	}

	window_ = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), data_.Title.data(), nullptr, nullptr);
	++s_GLFWWindowCount;

	glfwMakeContextCurrent(window_);
	const int glad_init_success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	CC_ASSERT(glad_init_success, "Could not initialize GLAD.");

	CC_INFO("OpenGL Info: ");
	CC_INFO("	Vendor: ", glGetString(GL_VENDOR));
	CC_INFO("	Renderer: ", glGetString(GL_RENDERER));
	CC_INFO("	Version: ", glGetString(GL_VERSION));

#ifdef CC_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;

		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
		CC_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Crystalline Carnival requires at least OpenGL version 3.3!");
#endif

	if (!window_)
		CC_FATAL("Could not create the window.");

	glfwSetWindowUserPointer(window_, &data_);
	glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	cursorEnabled_ = false;
	SetVSyncEnabled(true);
	glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#pragma region GLFW_CALLBACKS
	glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data.Width = width;
		data.Height = height;

		WindowResizeEvent event(width, height);

		data.EventCallback(event);
	});

	glfwSetWindowCloseCallback(window_, [](GLFWwindow* window)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		WindowCloseEvent event;
		data.EventCallback(event);
	});

	glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
		}
	});

	glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
		}
	});

	glfwSetScrollCallback(window_, [](GLFWwindow* window, double x_offset, double y_offset)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseScrolledEvent event({ static_cast<float>(x_offset), static_cast<float>(y_offset) });
		data.EventCallback(event);
	});

	glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double x_pos, double y_pos)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseMovedEvent event({ static_cast<float>(x_pos), static_cast<float>(y_pos) });
		data.EventCallback(event);
	});
#pragma endregion GLFW_CALLBACKS
}

void Window::Exit()
{
	glfwDestroyWindow(window_);
}

void Window::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(window_);
}

void Window::SetVSyncEnabled(bool enabled)
{
	glfwSwapInterval(enabled);

	data_.VSyncEnabled = enabled;
}

bool Window::IsVSyncEnabled() const
{
	return data_.VSyncEnabled;
}

void Window::SetCursorEnabled(bool enabled)
{
	if (!enabled)
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	cursorEnabled_ = enabled;
}

bool Window::IsCursorEnabled() const
{
	return cursorEnabled_;
}
