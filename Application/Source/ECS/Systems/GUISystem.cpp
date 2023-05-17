#include "GUISystem.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#include <examples/imgui_impl_opengl3.h>

static Coordinator* coordinator = Coordinator::Instance();
auto InventoryGUISystem::OnImGuiRender() -> void
{
    auto& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;

    constexpr float item_size_multiplier = 1.5f;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.0f);
    // ImGui::Begin("Inventory", 0, ImGuiWindowFlags_NoTitleBar );
    ImGui::Begin("Inventory", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize /*| ImGuiWindowFlags_NoMove*/ |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings);

    // NOTE - Convert RendererID to something ImGUI can interpret (image needs to be NOT FLIPPED)

    // if (ImGui::BeginTable("inventory", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX |
    //     ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_PreciseWidths))
    // if (ImGui::BeginTable("inventory", 6, ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX |
    //     ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_PreciseWidths))
    // {
    //     ImGui::TableNextRow();
    //     for (int i = 0; i < 6; ++i)
    //     {
    //         ImGui::TableSetColumnIndex(i);
    //         ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(emptyItemSprite_->GetTexture()->GetRendererID())),
    //             ImVec2(item_size_multiplier * static_cast<float>(emptyItemSprite_->GetWidth()),
    //                 item_size_multiplier * static_cast<float>(emptyItemSprite_->GetHeight())),
    //             ImVec2(emptyItemSprite_->GetTexCoordsArray()[0].x, emptyItemSprite_->GetTexCoordsArray()[0].y),
    //             ImVec2(emptyItemSprite_->GetTexCoordsArray()[2].x, emptyItemSprite_->GetTexCoordsArray()[2].y)
    //         );
    //     }
    //     ImGui::EndTable();
    // }
    ImGui::End();
    for (auto& e : entities)
    {
        auto& owned_by = coordinator->GetComponent<OwnedByComponent>(e);


    }
}