#include "PlayScene.h"

#include "EC/Components/MazeController.h"
#include "EC/Components/SpriteRenderer.h"
#include "EC/Components/TriggerCallback.h"
#include "EC/Components/Tilemap.h"
#include "EC/Components/TilemapRenderer.h"
#include "EC/Components/EnemyDisplay.h"
#include "EC/Components/BoxCollider2D.h"
#include "EC/Components/Pathfinder.h"

#include "Scene/SceneManager.h"

#include "Game/GameManager.h"


#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <array>
#include "Scene/Scene.h"
#include "EC/Components/PlayerController.h"

PlayScene::PlayScene()
    : Scene("PlayScene")
    , ppiMultiplier_(glm::vec2(8, 8) / 32.0f)
    , ironOreSprite_(CreateRef<Texture2D>("Assets/Images/Ore2.png"))
    , arrowSprite_(CreateRef<Texture2D>("Assets/Images/60934.png"))
    , witchSprite_(CreateRef<Texture2D>("Assets/Spritesheets/Blue_witch/B_witch_idle.png"))
    , banditSprite_(CreateRef<Texture2D>("Assets/Spritesheets/Idle.png"))
    , playerSprite_(CreateRef<Texture2D>("Assets/Images/rockett-young-pixel-lulu.png"))
    , enemySprite_(CreateRef<Texture2D>("Assets/Images/RemovedBGandShadows.png"))
{
    SetupTilemap();
    SetupVisiblityTilemap();
    SetupPlayer();
    SetupDisplay();
    SetupEnemies();

    GenerateMaze();

    GameManager::Instance()->StartGame();
    // CreateMines();
    // SetupMiners();
    // SetupWitches();
    // SetupKnights();
    // SetupBandits();
}

void PlayScene::GenerateMaze()
{
    tilemapGameObject_->GetComponent<MazeController>()
        ->Generate(tilemap_->WorldToLocal(
            playerGameObject_->GetTransform().GetPosition()));
}

void PlayScene::InjectTilemapEntityColor(Ref<TilemapEntity> tilemapEntity)
{
    auto gameObject = CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(2.f));
    gameObject->AddComponent<SpriteRenderer>();
    auto tilemapEntityColor = gameObject->AddComponent<TilemapEntityColor>();
    tilemapEntityColor->SetOffset({0, 6, 0});
    tilemapEntity->SetColorObject(tilemapEntityColor);
    tilemapEntityColor->SetTransformToFollow(&tilemapEntity->GetTransform());
}

void PlayScene::SetupPlayer()
{
    const float multiplier = 6.0f / playerSprite_->GetWidth();

    playerGameObject_ = CreateGameObject();
    playerGameObject_->SetTag("Player");

    auto playerSpriteRenderer = 
        playerGameObject_->AddComponent<SpriteRenderer>();
    playerSpriteRenderer->SetTexture(playerSprite_);
    playerSpriteRenderer->SetNativeSize();

    playerGameObject_->GetTransform().Scale(multiplier);

    auto playerController = 
        playerGameObject_->AddComponent<PlayerController>();
    playerController->SetTilemap(tilemap_);
    playerController->SetVisibilityTilemap(visibilityTilemap_);
    playerController->SetVisibilityRange(2); 
    InjectTilemapEntityColor(playerController);
    GameManager::Instance()->AddTilemapEntity(playerController);
}

void PlayScene::SetupEnemies()
{
    constexpr std::array<glm::ivec2, 2> enemySpawns
    {
        {
            { 1, 1 },
            { 23, 23 }
        }
    };

    for (int i = 0; i < 2; ++i)
    {
        const float multiplier = 8.0f / enemySprite_->GetWidth();

        auto enemyGameObject = CreateGameObject();

        auto enemySpriteRenderer = 
            enemyGameObject->AddComponent<SpriteRenderer>();
        enemySpriteRenderer->SetTexture(enemySprite_);
        enemySpriteRenderer->SetNativeSize();
        enemySpriteRenderer->SetSortingOrder(3);

        enemyGameObject->GetTransform().Scale(multiplier);

        enemyGameObject->GetTransform().SetPosition(
            tilemap_->LocalToWorld(enemySpawns[i]));

        auto enemyController =
            enemyGameObject->AddComponent<EnemyController>();
        enemyController->SetTilemap(tilemap_)
            ->SetVisibilityTilemap(visibilityTilemap_)
            ->SetVisibilityRange(4)
            ->SetStartHealth(50.0f);
        InjectTilemapEntityColor(enemyController);
        GameManager::Instance()->AddTilemapEntity(enemyController);
    }
}

void PlayScene::SetupVisiblityTilemap()
{
    // TODO - Change tilemapGameObject_ to visiblityTilemapObject_
    auto blackTile = CreateRef<Texture2D>("Assets/Images/Untitled.png");
    constexpr glm::vec2 tilemapTileSize = glm::vec2(32, 32);

    visibilityTilemapGameObject_ = CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(1.f));
    visibilityTilemapGameObject_->AddComponent<Tilemap>("Assets/Maps/VisibilityMap.csv", 
        "Assets/Maps/FillerMap.csv")
        // NOTE - Comment this PushTexture call to see visibility
        ->PushTexture(CreateRef<SubTexture2D>(blackTile, glm::vec2(), 
            glm::vec2(1.0f), blackTile->GetWidth(), blackTile->GetHeight()))
        ->GetGameObject().GetComponent<TilemapRenderer>()
        ->SetSortingOrder(1);

    visibilityTilemap_ = visibilityTilemapGameObject_->GetComponent<Tilemap>();
    visibilityTilemap_->tileSize = glm::vec2(8, 8);
    auto bounds = visibilityTilemap_->GetBounds();
    visibilityTilemapGameObject_->GetTransform().SetPosition(
        glm::vec3(bounds.x * -0.5f, bounds.y * -0.5f, 0));
    visibilityTilemap_->ResetAllTiles(1);
}

void PlayScene::SetupTilemap()
{
    auto tilemapSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/Tilemap/TX Tileset Grass.png");

    constexpr glm::vec2 tilemapTileSize = glm::vec2(32, 32);

    auto grassTileTopLeft = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(1, 7), tilemapTileSize);
    auto grassTileTopMiddle = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(2, 7), tilemapTileSize);
    auto flowerTileTopLeft = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(4, 7), tilemapTileSize);
    auto fullStoneGrassTile = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(0, 3), tilemapTileSize);

    tilemapGameObject_ = CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(1.f));
        tilemapGameObject_->AddComponent<Tilemap>("Assets/Maps/TwentyFiveMap.csv", "Assets/Maps/TwentyFiveMapTypes.csv")
        ->PushTexture(grassTileTopLeft)
        ->PushTexture(grassTileTopMiddle)
        ->PushTexture(flowerTileTopLeft)
        ->PushTexture(fullStoneGrassTile)
        ->GetGameObject().GetComponent<TilemapRenderer>()
        ->SetSortingOrder(-2);

    tilemap_ = tilemapGameObject_->GetComponent<Tilemap>();
    tilemap_->tileSize = glm::vec2(8, 8);
    // tilemapGameObject_->GetComponent<Tilemap>()->SetDataBounds({0, 0}, {25, 25});
    auto bounds = tilemap_->GetBounds();
    tilemapGameObject_->GetTransform().SetPosition(glm::vec3(bounds.x * -0.5f, bounds.y * -0.5f, 0));
    tilemapGameObject_->AddComponent<MazeController>()
        ->SetTilemap(tilemap_)
        ->SetWallTextureIndex(4)
        ->SetEmptyTextureIndex(2);
    tilemapGameObject_->AddComponent<Pathfinder>();
    // tilemapGameObject_->AddComponent<Pathfinder>();
    // tilemapGameObject_->SetTag("Pathfinder");
}

void PlayScene::SetupDisplay()
{
    auto gameObject = CreateGameObject();
    enemyDisplay_ = gameObject->AddComponent<EnemyDisplay>();
}
