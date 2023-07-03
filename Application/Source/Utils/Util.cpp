#include "Util.h"
#include <examples/imgui_impl_opengl3.h>

void Utility::ImGuiImage(const Ref<Texture2D> &icon, float width, float height)
{
    ImGui::Image(reinterpret_cast<ImTextureID>(
        static_cast<intptr_t>(icon->GetRendererID())), ImVec2(width, height));
}

void Utility::ImGuiImage(const Ref<Texture2D> &icon, float imageSizeMultiplier)
{
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(icon->GetRendererID())),
        ImVec2(imageSizeMultiplier * static_cast<float>(icon->GetWidth()),
            imageSizeMultiplier * static_cast<float>(icon->GetHeight()))
    );
}

void Utility::ImGuiImage(const Ref<SubTexture2D> &icon, float imageSizeMultiplier)
{
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(icon->GetTexture()->GetRendererID())),
        ImVec2(imageSizeMultiplier * static_cast<float>(icon->GetWidth()),
            imageSizeMultiplier * static_cast<float>(icon->GetHeight())),
        ImVec2(icon->GetTexCoordsArray()[0].x, icon->GetTexCoordsArray()[0].y),
        ImVec2(icon->GetTexCoordsArray()[2].x, icon->GetTexCoordsArray()[2].y)
    );
}

