#include "GUISystem.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

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

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Inventory", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize /*| ImGuiWindowFlags_NoMove*/ |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings);

    // NOTE - Convert RendererID to something ImGUI can interpret (image needs to be NOT FLIPPED)
    // NOTE - There should only probably be 6 entities with OwnedBy component, because the inventory is only 6 slots.
    if (ImGui::BeginTable("inventory", InventorySize, ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX |
        ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_PreciseWidths))
    {
        ImGui::TableNextRow();
        size_t i = 0;
        for (auto& e : entities)
        {
            ImGui::TableSetColumnIndex(i);
            auto& item = coordinator->GetComponent<ItemComponent>(e);
            ImGuiImage(item.Icon ? item.Icon : emptySpriteTexture_);
            ++i;
        }

        for (; i < InventorySize; ++i)
        {
            ImGui::TableSetColumnIndex(i);
            ImGuiImage(emptySpriteTexture_);
        }

        ImGui::EndTable();
    }


    ImGui::End();
}

auto InventoryGUISystem::ImGuiImage(const Ref<Texture2D>& icon, float itemSizeMultiplier) -> void
{
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(icon->GetRendererID())),
        ImVec2(itemSizeMultiplier * static_cast<float>(icon->GetWidth()),
            itemSizeMultiplier * static_cast<float>(icon->GetHeight()))
    );
}

auto InventoryGUISystem::ImGuiImage(const Ref<SubTexture2D> &icon, float itemSizeMultiplier) -> void
{
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(icon->GetTexture()->GetRendererID())),
        ImVec2(itemSizeMultiplier * static_cast<float>(icon->GetWidth()),
            itemSizeMultiplier * static_cast<float>(icon->GetHeight())),
        ImVec2(icon->GetTexCoordsArray()[0].x, icon->GetTexCoordsArray()[0].y),
        ImVec2(icon->GetTexCoordsArray()[2].x, icon->GetTexCoordsArray()[2].y)
    );
}
