#include "PlayScene.h"

#include "Core/KeyCodes.h"

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
#include "Utils/Input.h"

PlayScene::PlayScene()
    : Scene("PlayScene")
    , ppiMultiplier_(glm::vec2(8, 8) / 32.0f)
    , ironOreSprite_(CreateRef<Texture2D>("Assets/Images/Ore2.png"))
    , arrowSprite_(CreateRef<Texture2D>("Assets/Images/60934.png"))
    , witchSprite_(CreateRef<Texture2D>("Assets/Spritesheets/Blue_witch/B_witch_idle.png"))
    , banditSprite_(CreateRef<Texture2D>("Assets/Spritesheets/Idle.png"))
    , playerSprite_(CreateRef<Texture2D>("Assets/Images/rockett-young-pixel-lulu.png"))
    , enemySprite_(CreateRef<Texture2D>("Assets/Images/RemovedBGandShadows.png"))
    , endFlagSprite_(CreateRef<Texture2D>("Assets/Images/finishflag.png"))
{
    // CreateMines();
    // SetupMiners();
    // SetupWitches();
    // SetupKnights();
    // SetupBandits();
}

void PlayScene::Start()
{
    SetupTilemap();
    SetupEndFlag();
    SetupVisiblityTilemap();
    SetupPlayer();
    SetupDisplay();
    SetupEnemies();

    GenerateMaze();

    GameManager::Instance()->StartGame();

    Scene::Start();
}

void PlayScene::Update(Timestep ts)
{
    static auto gameManager = GameManager::Instance();
    gameManager->SetInUpdate(true);
    Scene::Update(ts);
    gameManager->SetInUpdate(false);
    static Input* input = Input::Instance();
    if (input->IsKeyPressed(Key::Enter))
    {
        GameManager::Instance()->NewGame();
    }
}

void PlayScene::GenerateMaze()
{
    endFlagObject_->GetTransform().SetPosition(
        tilemap_->LocalToWorld(
        tilemapGameObject_->GetComponent<MazeController>()
        ->Generate(tilemap_->WorldToLocal(
            playerGameObject_->GetTransform().GetPosition()),
            10)));
}

void PlayScene::InjectTilemapEntityColor(Ref<TilemapEntity> tilemapEntity)
{
    auto gameObject = CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(2.f));
    gameObject->AddComponent<SpriteRenderer>()->SetSortingOrder(15);
    auto tilemapEntityColor = gameObject->AddComponent<TilemapEntityColor>();
    tilemapEntityColor->SetOffset({0, 6, 0});
    tilemapEntity->SetColorObject(tilemapEntityColor);
    tilemapEntityColor->SetTransformToFollow(&tilemapEntity->GetTransform());
}

void PlayScene::SetupPlayer()
{
    const float multiplier = 6.0f / playerSprite_->GetWidth();

    playerGameObject_ = CreateGameObject();
    playerGameObject_->GetTransform().SetPosition(
        tilemap_->LocalToWorld(glm::ivec2(5 + (rand() % 7) * 2, 
        5 + (rand() % 7) * 2)));
    playerGameObject_->SetTag("Player");

    auto playerSpriteRenderer = 
        playerGameObject_->AddComponent<SpriteRenderer>();
    playerSpriteRenderer->SetTexture(playerSprite_);
    playerSpriteRenderer->SetNativeSize();

    playerGameObject_->GetTransform().Scale(multiplier);

    auto playerController = 
        playerGameObject_->AddComponent<PlayerController>();
    playerController->SetStartHealth(100.0f);
    playerController->SetTilemap(tilemap_);
    playerController->SetVisibilityTilemap(visibilityTilemap_);
    playerController->SetVisibilityRange(4); 
    InjectTilemapEntityColor(playerController);
    GameManager::Instance()->AddTilemapEntity(playerController);
}

void PlayScene::SetupEnemies()
{

    std::array<glm::ivec2, 3> enemySpawns
    {
        // {
        //     { 1, 1 },
        //     { 23, 23 },
        //     { 11, 13 }
        // }
    };

    enemySpawns[0] = { 1 + (rand() % 12) * 2, 1 + (rand() % 2) * 2 };
    enemySpawns[1] = { 1 + (rand() % 12) * 2, 
        Tilemap::MaxVerticalLength - 2 - ((rand() % 2) * 2) };
    enemySpawns[2] = { 1 + (rand() % 2) * 2, 1 + (rand() % 12) * 2 };

    for (uint32_t i = 0; i < enemySpawns.size(); ++i)
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
            ->SetDamage(3.0f)
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
    auto waterSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/Ocean_SpriteSheet.png");

    constexpr glm::vec2 tilemapTileSize = glm::vec2(32, 32);

    auto grassTileTopLeft = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(1, 7), tilemapTileSize);
    auto grassTileTopMiddle = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(2, 7), tilemapTileSize);
    auto flowerTileTopLeft = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(4, 7), tilemapTileSize);
    auto fullStoneGrassTile = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(0, 3), tilemapTileSize);
    auto halfStoneGrassTile = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(1, 0), tilemapTileSize);
    auto waterTile = SubTexture2D::CreateFromCoords(
        waterSpritesheet, glm::vec2(0, 0), tilemapTileSize);

    tilemapGameObject_ = CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(1.f));
        tilemapGameObject_->AddComponent<Tilemap>("Assets/Maps/TwentyFiveMap.csv", "Assets/Maps/TwentyFiveMapTypes.csv")
        ->PushTexture(grassTileTopLeft)
        ->PushTexture(grassTileTopMiddle)
        ->PushTexture(flowerTileTopLeft)
        ->PushTexture(fullStoneGrassTile)
        ->PushTexture(waterTile)
        ->PushTexture(halfStoneGrassTile)
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
        ->SetEmptyTextureIndex(2)
        ->SetWaterTextureIndex(5)
        ->SetBrokenWallTextureIndex(6);
    tilemapGameObject_->AddComponent<Pathfinder>();
    // tilemapGameObject_->AddComponent<Pathfinder>();
    // tilemapGameObject_->SetTag("Pathfinder");
}

void PlayScene::SetupEndFlag()
{
    endFlagObject_ = CreateGameObject();
    auto spriteRenderer = endFlagObject_->AddComponent<SpriteRenderer>();
    spriteRenderer->SetTexture(endFlagSprite_);
    spriteRenderer->SetSortingOrder(20);
    spriteRenderer->SetNativeSize();
    endFlagObject_->GetTransform().Scale(0.01f);
}

void PlayScene::SetupDisplay()
{
    auto gameObject = CreateGameObject();
    enemyDisplay_ = gameObject->AddComponent<EnemyDisplay>();
}
