#pragma once
#include "Scene/Scene.h"
#include "Core/Core.h"
#include "Renderer/Texture.h"

#include "EC/Components/MineController.h"
#include "EC/Components/EnemyDisplay.h"

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
    Ref<Texture2D> arrowSprite_;
    Ref<Texture2D> minerSprite_;
    Ref<Texture2D> knightSprite_;
    Ref<Texture2D> witchSprite_;
    Ref<Texture2D> banditSprite_;
    Ref<Texture2D> playerSprite_;
    Ref<Texture2D> enemySprite_;

    const glm::vec2 ppiMultiplier_;
    Ref<GameObject> tilemapGameObject_;
    Ref<GameObject> visibilityTilemapGameObject_;
    Ref<EnemyDisplay> enemyDisplay_;
    Ref<MineController> mineController_;
    Ref<Tilemap> tilemap_;
    Ref<Tilemap> visibilityTilemap_;

    Ref<GameObject> playerGameObject_;

    void GenerateMaze();

    void SetupPlayer();
    void SetupEnemies();
    void SetupVisiblityTilemap();
    void SetupTilemap();
    void SetupMiners();
    void SetupWitches();
    void CreateMines();
    void SetupDisplay();
    void SetupKnights();
    void SetupBandits();

    std::vector<Ref<GameObject>> CreateOrePile(uint32_t x, uint32_t y);
};
