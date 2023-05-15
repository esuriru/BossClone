#pragma once
#include "Core/Layer.h"
#include "Events/Event.h"


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
    bool blockEvents_ = true;
};