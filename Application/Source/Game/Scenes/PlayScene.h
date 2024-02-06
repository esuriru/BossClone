#pragma once
#include "Scene/Scene.h"
#include "Core/Core.h"
#include "Renderer/Texture.h"

#include "EC/Components/MineController.h"
#include "EC/Components/EnemyDisplay.h"
#include "EC/Components/TilemapEntity.h"
#include "EC/Components/TilemapEntityColor.h"

#include <vector>

class PlayScene : public Scene
{
public:
    PlayScene();

    inline virtual std::string GetName() override
    {
        return "PlayScene";
    }

    void Start() override;
    void Update(Timestep ts) override;

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
    Ref<Texture2D> endFlagSprite_;

    const glm::vec2 ppiMultiplier_;
    Ref<GameObject> tilemapGameObject_;
    Ref<GameObject> visibilityTilemapGameObject_;
    Ref<GameObject> endFlagObject_;
    Ref<EnemyDisplay> enemyDisplay_;
    Ref<MineController> mineController_;
    Ref<Tilemap> tilemap_;
    Ref<Tilemap> visibilityTilemap_;

    Ref<GameObject> playerGameObject_;

    void GenerateMaze();

    void InjectTilemapEntityColor(Ref<TilemapEntity> tilemapEntity);
    void SetupPlayer();
    void SetupEnemies();
    void SetupVisiblityTilemap();
    void SetupTilemap();
    void SetupEndFlag();
    void SetupDisplay();
};
