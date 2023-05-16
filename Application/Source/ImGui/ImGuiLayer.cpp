#include "ImGuiLayer.h"

#include "imgui.h"
#include <examples/imgui_impl_opengl3.h>
#include "Core/KeyCodes.h"

#include "Core/Application.h"
#include "Events/EventDispatcher.h"

ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer")
{
}

auto ImGuiLayer::Begin() -> void
{
    ImGuiIO& io = ImGui::GetIO();
    Application* app = Application::Instance();
    io.DisplaySize = ImVec2(static_cast<float>(app->GetWindow().GetWidth()), static_cast<float>(app->GetWindow().GetHeight()));

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

auto ImGuiLayer::End() -> void
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

auto ImGuiLayer::OnUpdate(Timestep ts) -> void 
{
}

auto ImGuiLayer::OnAttach() -> void
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    // TODO - Remove
    io.KeyMap[ImGuiKey_Tab] = Key::Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = Key::Left;
    io.KeyMap[ImGuiKey_RightArrow] = Key::Right;
    io.KeyMap[ImGuiKey_UpArrow] = Key::Up;
    io.KeyMap[ImGuiKey_DownArrow] = Key::Down;
    io.KeyMap[ImGuiKey_PageUp] = Key::PageUp;
    io.KeyMap[ImGuiKey_PageDown] = Key::PageDown;
    io.KeyMap[ImGuiKey_Home] = Key::Home;
    io.KeyMap[ImGuiKey_End] = Key::End;
    io.KeyMap[ImGuiKey_Insert] = Key::Insert;
    io.KeyMap[ImGuiKey_Delete] = Key::Delete;
    io.KeyMap[ImGuiKey_Backspace] = Key::Backspace;
    io.KeyMap[ImGuiKey_Space] = Key::Space;
    io.KeyMap[ImGuiKey_Enter] = Key::Enter;
    io.KeyMap[ImGuiKey_Escape] = Key::Escape;
    io.KeyMap[ImGuiKey_KeyPadEnter] = Key::KPEnter;
    io.KeyMap[ImGuiKey_A] = Key::A;
    io.KeyMap[ImGuiKey_C] = Key::C;
    io.KeyMap[ImGuiKey_V] = Key::V;
    io.KeyMap[ImGuiKey_X] = Key::X;
    io.KeyMap[ImGuiKey_Y] = Key::Y;
    io.KeyMap[ImGuiKey_Z] = Key::Z;


    // constexpr float fontSize = 18.0f;
    SetDarkTheme();

    // Application* app = Application::Instance();
    // GLFWwindow* window = static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());

    // ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

auto ImGuiLayer::OnDetach() -> void 
{
}

auto ImGuiLayer::OnEvent(Event &e) -> void 
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseButtonPressedEvent>(CC_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonPressedEvent));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(CC_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonReleased));
    dispatcher.Dispatch<MouseMovedEvent>(CC_BIND_EVENT_FUNC(ImGuiLayer::OnMouseMovedEvent));
    dispatcher.Dispatch<MouseScrolledEvent>(CC_BIND_EVENT_FUNC(ImGuiLayer::OnMouseScrolledEvent));
    dispatcher.Dispatch<KeyPressedEvent>(CC_BIND_EVENT_FUNC(ImGuiLayer::OnKeyPressedEvent));
    dispatcher.Dispatch<KeyReleasedEvent>(CC_BIND_EVENT_FUNC(ImGuiLayer::OnKeyReleasedEvent));
    dispatcher.Dispatch<WindowResizeEvent>(CC_BIND_EVENT_FUNC(ImGuiLayer::OnWindowResizeEvent));
    // if (blockEvents_)
    // {
    //     auto& io = ImGui::GetIO();
    //     e.Handled = e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
    //     e.Handled = e.IsInCategory(EventCategoryMouse) & io.WantCaptureKeyboard;
    // }
}


auto ImGuiLayer::SetDarkTheme() -> void
{
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    
    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}

auto ImGuiLayer::GetActiveWidgetID() const -> uint32_t
{
    return 0;
}

auto ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent &e) -> bool
{
    auto& io = ImGui::GetIO();
    io.MouseDown[e.GetMouseButton()] = true;

    return false;
}

auto ImGuiLayer::OnMouseButtonReleased(MouseButtonReleasedEvent &e) -> bool
{
    auto& io = ImGui::GetIO();
    io.MouseDown[e.GetMouseButton()] = false;

    return false;
}

auto ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent &e) -> bool
{
    auto& io = ImGui::GetIO();
    auto coords = e.GetCoords();
    io.MousePos = ImVec2(coords.x, coords.y);

    return false;
}

auto ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent &e) -> bool
{
    auto& io = ImGui::GetIO();
    auto offset = e.GetOffset();
    io.MouseWheelH += offset.x;
    io.MouseWheel += offset.y;

    return false;
}

auto ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent &e) -> bool
{
    auto& io = ImGui::GetIO();
    io.KeysDown[e.GetKeyCode()] = true;

    io.KeyCtrl = io.KeysDown[Key::LeftControl] || io.KeysDown[Key::RightControl];
    io.KeyAlt = io.KeysDown[Key::LeftAlt] || io.KeysDown[Key::RightAlt];
    io.KeyShift = io.KeysDown[Key::LeftShift] || io.KeysDown[Key::RightShift];
    io.KeySuper = io.KeysDown[Key::LeftSuper] || io.KeysDown[Key::RightSuper];

    return false;
}

auto ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent &e) -> bool
{
    auto& io = ImGui::GetIO();
    io.KeysDown[e.GetKeyCode()] = false;

    return false;
}

auto ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent &e) -> bool
{
    auto& io = ImGui::GetIO();
    int keycode = static_cast<unsigned int>(e.GetKeyCode());
    if (keycode > 0 && keycode < 0x10000)
        io.AddInputCharacter(static_cast<unsigned short>(keycode));

    return false;
}

auto ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent &e) -> bool
{
    auto& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    return false;
}
