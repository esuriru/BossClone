#pragma once

#include "ECS/System.h"
#include "Core/Core.h"
#include "Renderer/SubTexture2D.h"
#include "Renderer/Texture.h"

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