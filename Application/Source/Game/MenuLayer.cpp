#include "MenuLayer.h"
#include <examples/imgui_impl_opengl3.h> 
#include "ECS/Systems/GUISystem.h"
#include "GameManager.h"
#include "Audio/Transition.h"
#include "Utils/MusicPlayer.h"
#include "Core/Core.h"
#include "Utils/SoundController.h"

MenuLayer::MenuLayer()
    : Layer("MenuLayer")
{
    mapPreview_ = CreateRef<Texture2D>("Assets/Images/Preview.png", false);
    firstTime_ = true;
}

auto MenuLayer::OnAttach() -> void 
{
    GameManager::Instance()->Init();
}

auto MenuLayer::OnDetach() -> void 
{
    firstTime_ = false;
}

auto MenuLayer::OnUpdate(Timestep ts) -> void 
{
}

auto MenuLayer::OnEvent(Event &e) -> void 
{
}

auto MenuLayer::OnImGuiRender() -> void 
{
    auto io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("Menu", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetCursorPosX(100);
    ImGui::SetCursorPosY(100);
    Utility::ImGuiImage(mapPreview_, 0.5f);

    ImGui::SetCursorPosY(330);
    ImGui::SetCursorPosX(175);

    ImGui::SetWindowFontScale(3.0f);
    ImGui::Text("1-1");

    ImGui::SetCursorPosX(160);
    ImGui::SetCursorPosY(400);
    if (ImGui::Button("Play")) 
    {
        GameManager::Instance()->ChangeState(GameState::PlayingLevel);
    }
    if (ImGui::IsItemHovered())
    {
        if (!buttonHovered_)
        {
            buttonHovered_ = true;
            SoundController::Instance()->PlaySoundByID(1, true);
        }
    }
    else
    {
        buttonHovered_ = false;
    }

    ImGui::SetCursorPosX(88);
    ImGui::SetCursorPosY(450);
    ImGui::SetWindowFontScale(2.0f);

    if (!firstTime_)
    {
        ImGui::Text("Best timing: %.2fs", GameManager::Instance()->GetBestTime());
    }
    else
    {
        ImGui::Text("Best timing: NONE");
    }

    ImGui::End();
}
