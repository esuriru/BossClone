#pragma once

#include "ECS/System.h"
#include "Core/Core.h"
#include "Renderer/SubTexture2D.h"


class InventoryGUISystem : public System
{
public:
    InventoryGUISystem();
    auto OnImGuiRender() -> void;

    // TODO - Should this be left as static?
    static Ref<Texture2D> ItemSpritesheet;

private:
    // TODO - Add custom UV coordinates
    auto ImGuiImage(const Ref<Texture2D>& icon, float itemSizeMultiplier = 1.5f) -> void;
    auto ImGuiImage(const Ref<SubTexture2D>& icon, float itemSizeMultiplier = 1.5f) -> void;

    Ref<SubTexture2D> emptySpriteTexture_;
};