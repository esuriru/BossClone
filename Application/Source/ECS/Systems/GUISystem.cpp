#include "GUISystem.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Utils/Util.h"
#include "Utils/Input.h"
#include "Core/KeyCodes.h"

#include "Core/Application.h"

#include <examples/imgui_impl_opengl3.h>

Ref<Texture2D> InventoryGUISystem::ItemSpritesheet;
static Coordinator* coordinator = Coordinator::Instance();


InventoryGUISystem::InventoryGUISystem()
{
    ItemSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/ShikashiFantasyIconPackV2/BG7Icons.png", false);
    emptySpriteTexture_ = SubTexture2D::CreateFromCoords(ItemSpritesheet, glm::vec2(10, 1), glm::vec2(32, 32));
}

auto InventoryGUISystem::OnImGuiRender() -> void
{
    auto& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;

    constexpr float DefaultCellLength = 1.75f * 32.f;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Inventory", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings);
    
        ImGui::SetWindowSize(ImVec2(DefaultCellLength * 7, DefaultCellLength + 20.f));

        // NOTE - Convert RendererID to something ImGUI can interpret (image needs to be NOT FLIPPED)
        // NOTE - There should only probably be 6 entities with OwnedBy component, because the inventory is only 6 slots.
        if (ImGui::BeginTable("inventory", InventorySize, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, DefaultCellLength);
            ImGui::TableNextRow();
            size_t i = 0;

            // float tableWidth = ImGui::GetContentRegionAvail().x; 
            // float cellPaddingX = (tableWidth - DefaultCellLength * 6) * 0.5f;
            float cellPaddingX = (1.75f * 32) - (1.5f * 32);
            auto& inventory = coordinator->GetComponent<InventoryComponent>(*helperSystem->entities.begin());
            for (const auto& e : inventory.Items)
            {
                auto& item = coordinator->GetComponent<ItemComponent>(e);

                ImGui::TableSetColumnIndex(i);
                if (e != inventory.CurrentlyHolding)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cellPaddingX);
                Utility::ImGuiImage(item.Icon ? item.Icon : emptySpriteTexture_,
                    (e == inventory.CurrentlyHolding) ? 1.75f : 1.5f);
                ++i;
            }

            for (; i < InventorySize; ++i)
            {
                ImGui::TableSetColumnIndex(i);
                if (i != inventory.CurrentlyHoldingIndex)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cellPaddingX);
                Utility::ImGuiImage(emptySpriteTexture_, (i == inventory.CurrentlyHoldingIndex) ? 1.75f : 1.5f);
            }

            ImGui::EndTable();
        }


    ImGui::End();
}

PlayerHealthGUISystem::PlayerHealthGUISystem()
{
    baseHealthBar_ = CreateRef<Texture2D>("Assets/Images/PixQuestGUI/GUI Assets/Status Bar Horizontal (Round)/Base.png");
    emptyHealthBar_ = CreateRef<Texture2D>("Assets/Images/PixQuestGUI/GUI Assets/Status Bar Horizontal (Round)/Empty.png");
    fullHealthBar_ = CreateRef<Texture2D>("Assets/Images/PixQuestGUI/GUI Assets/Status Bar Horizontal (Round)/FullRed.png");
}

auto PlayerHealthGUISystem::OnImGuiRender() -> void
{
    CC_ASSERT(entities.size() <= 1, "There should be no more than one player.");
    Entity player = *entities.begin();
    static Application* app = Application::Instance();

    auto& health_comp = coordinator->GetComponent<HealthComponent>(player);

    // TODO - Make this change ONLY on damage events.
    float percentage = glm::clamp(health_comp.Health / health_comp.MaxHealth, 0.f, 1.f);

    constexpr float HealthBarRightOffset = 220.f;
    constexpr float HealthBarSizeMultiplier = 4.f;

    ImGui::SetNextWindowPos(ImVec2(app->GetWindowWidth() - HealthBarRightOffset, 0), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Health Bar", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings);

        Utility::ImGuiImage(baseHealthBar_, HealthBarSizeMultiplier);
        ImGui::SameLine();
        ImGui::SetCursorPosX(  
            percentage * (baseHealthBar_->GetWidth() * HealthBarSizeMultiplier)
            - (baseHealthBar_->GetWidth() * HealthBarSizeMultiplier * 0.5f)
        );
        Utility::ImGuiImage(percentage <= 0.5f ? emptyHealthBar_ : fullHealthBar_, HealthBarSizeMultiplier);

    ImGui::End();
}

auto Utility::ImGuiImage(const Ref<Texture2D> &icon, float width, float height) -> void
{
    ImGui::Image(reinterpret_cast<ImTextureID>(
        static_cast<intptr_t>(icon->GetRendererID())), ImVec2(width, height));
}

auto Utility::ImGuiImage(const Ref<Texture2D> &icon, float imageSizeMultiplier) -> void
{
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(icon->GetRendererID())),
        ImVec2(imageSizeMultiplier * static_cast<float>(icon->GetWidth()),
            imageSizeMultiplier * static_cast<float>(icon->GetHeight()))
    );
}

auto Utility::ImGuiImage(const Ref<SubTexture2D> &icon, float imageSizeMultiplier) -> void
{
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(icon->GetTexture()->GetRendererID())),
        ImVec2(imageSizeMultiplier * static_cast<float>(icon->GetWidth()),
            imageSizeMultiplier * static_cast<float>(icon->GetHeight())),
        ImVec2(icon->GetTexCoordsArray()[0].x, icon->GetTexCoordsArray()[0].y),
        ImVec2(icon->GetTexCoordsArray()[2].x, icon->GetTexCoordsArray()[2].y)
    );
}
