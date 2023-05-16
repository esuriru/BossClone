#pragma once
#include "Core/Layer.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"


class ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer() = default;

    virtual auto OnUpdate(Timestep ts) -> void override;
    virtual auto OnAttach() -> void override;
    virtual auto OnDetach() -> void override;
    virtual auto OnEvent(Event& e) -> void override;

    auto Begin() -> void;
    auto End() -> void;

    auto BlockEvents(bool block) -> void { blockEvents_ = block; };

    auto SetDarkTheme() -> void;

    auto GetActiveWidgetID() const -> uint32_t;

private:
    auto OnMouseButtonPressedEvent(MouseButtonPressedEvent& e) -> bool;
    auto OnMouseButtonReleased(MouseButtonReleasedEvent& e) -> bool;
    auto OnMouseMovedEvent(MouseMovedEvent& e) -> bool;
    auto OnMouseScrolledEvent(MouseScrolledEvent& e) -> bool;

    auto OnKeyPressedEvent(KeyPressedEvent& e) -> bool;
    auto OnKeyReleasedEvent(KeyReleasedEvent& e) -> bool;
    auto OnKeyTypedEvent(KeyTypedEvent& e) -> bool;

    auto OnWindowResizeEvent(WindowResizeEvent& e) -> bool;

    bool blockEvents_ = true;
};