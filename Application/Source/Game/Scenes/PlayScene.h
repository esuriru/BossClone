#pragma once
#include "Scene/Scene.h"
#include "Core/Core.h"
#include "Renderer/Texture.h"

#include "EC/Components/MineController.h"

#include <vector>

class PlayScene : public Scene
{
public:
    PlayScene();

    inline virtual std::string GetName() override
    {
        return "PlayScene";
    }

private:
    Ref<Texture2D> nareLogoTexture_;
    Ref<Texture2D> terrainSpritesheet_;
    Ref<Texture2D> ironOreSprite_;

    const glm::vec2 ppiMultiplier_;
    Ref<GameObject> tilemapGameObject_;
    Ref<MineController> mineController_;

    void SetupTilemap();
    void SetupMiners();
    void CreateMines();
    std::vector<Ref<GameObject>> CreateOrePile(uint32_t x, uint32_t y);
};
