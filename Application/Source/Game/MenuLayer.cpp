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
    gear_ = CreateRef<Texture2D>("Assets/Images/gear.png", false);
    firstTime_ = true;
}

auto MenuLayer::OnAttach() -> void 
{
    GameManager::Instance()->Init();
}

auto MenuLayer::OnDetach() -> void 
{
    firstTime_ = false;
    settingsOn_ = false;
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
    
    ImGui::SetCursorPosX(io.DisplaySize.x - 100);

    static SoundController* soundController = SoundController::Instance();
    static bool gearHovered = false;

    if (!settingsOn_)
    {
        if (ImGui::ImageButton(Utility::ImGuiImageTexture(gear_), ImVec2(80, 80)))
        {
            settingsOn_ = true;
        }
        if (ImGui::IsItemHovered())
        {
            if (!gearHovered)
            {
                gearHovered = true;
                soundController->PlaySoundByID(1, true);
            }
        }
        else
        {
            gearHovered = false;
        }
    }
    else
    {
        if (ImGui::ImageButton(Utility::ImGuiImageTexture(gear_), ImVec2(80, 80)))
        {
            settingsOn_ = false;
        }
        if (ImGui::IsItemHovered())
        {
            if (!gearHovered)
            {
                gearHovered = true;
                soundController->PlaySoundByID(1, true);
            }
        }
        else
        {
            gearHovered = false;
        }
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 450, 120));
        static MusicPlayer* player = MusicPlayer::Instance();
        ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings);
        float bgmMasterVolume_ = player->GetVolume();
        ImGui::SliderFloat("BGM Master volume", &bgmMasterVolume_, 0.0f, 1.0f);
        player->SetMasterVolume(bgmMasterVolume_);
        static SoundController* sc = SoundController::Instance();
        float soundVolume = sc->GetVolume();
        ImGui::SliderFloat("SFX Master volume", &soundVolume, 0.0f, 1.0f);
        sc->SetMasterVolume(soundVolume);
        ImGui::End();

    }

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
            soundController->PlaySoundByID(1, true);
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
