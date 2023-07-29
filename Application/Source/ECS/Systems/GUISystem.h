#pragma once

#include "ECS/System.h"
#include "Core/Core.h"
#include "Renderer/SubTexture2D.h"
#include "Renderer/Texture.h"

typedef void* ImTextureID;
namespace Utility
{
    auto ImGuiImage(const Ref<Texture2D>& icon, float imageSizeMultiplier = 1.5f) -> void;
    auto ImGuiImage(const Ref<Texture2D>& icon, float width, float height) -> void;
    auto ImGuiImage(const Ref<SubTexture2D> &icon, float imageSizeMultiplier = 1.5f) -> void;
    ImTextureID ImGuiImageTexture(const Ref<Texture2D>& icon);
    ImTextureID ImGuiImageTexture(const Ref<SubTexture2D>& icon);
}


// NOTE - This system is literally just to get the player entity for the inventory GUI to get the inventory component
class InventoryGUIHelperSystem : public System {};

class InventoryGUISystem : public System
{
public:
    InventoryGUISystem();
    auto OnImGuiRender() -> void;

    Ref<InventoryGUIHelperSystem> helperSystem;

    // TODO - Should this be left as static?
    static Ref<Texture2D> ItemSpritesheet;
private:
    Ref<SubTexture2D> emptySpriteTexture_;
};

class PlayerHealthGUISystem : public System
{
public:
    PlayerHealthGUISystem();
    auto OnImGuiRender() -> void;

private:
    Ref<Texture2D> baseHealthBar_;
    Ref<Texture2D> emptyHealthBar_;
    Ref<Texture2D> fullHealthBar_;

};