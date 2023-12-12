#include "PlayScene.h"

#include "EC/Components/SpriteRenderer.h"
#include "EC/Components/TriggerCallback.h"
#include "EC/Components/Tilemap.h"
#include "EC/Components/TilemapRenderer.h"
#include "EC/Components/MinerController.h"
#include "EC/Components/KnightController.h"
#include "EC/Components/WitchController.h"
#include "EC/Components/EnemyDisplay.h"
#include "EC/Components/BoxCollider2D.h"
#include "EC/Components/Pathfinder.h"

#include "Scene/SceneManager.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <array>
#include "Scene/Scene.h"

PlayScene::PlayScene()
    : Scene("PlayScene")
    , ppiMultiplier_(glm::vec2(8, 8) / 32.0f)
    , ironOreSprite_(CreateRef<Texture2D>("Assets/Images/Ore2.png"))
    , arrowSprite_(CreateRef<Texture2D>("Assets/Images/60934.png"))
{
    SetupTilemap();
    SetupDisplay();
    CreateMines();
    SetupMiners();
    SetupWitches();
    SetupKnights();
}

void PlayScene::SetupTilemap()
{
    auto tilemapSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/Tilemap/TX Tileset Grass.png");

    constexpr glm::vec2 tilemapTileSize = glm::vec2(32, 32);

    auto grassTileTopLeft = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(1, 7), tilemapTileSize);
    auto grassTileTopMiddle = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(2, 7), tilemapTileSize);
    auto flowerTileTopLeft = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(4, 7), tilemapTileSize);

    tilemapGameObject_ = CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(1.f));
        tilemapGameObject_->AddComponent<Tilemap>("Assets/Maps/TwentyFiveMap.csv", "Assets/Maps/TwentyFiveMapTypes.csv")
        ->PushTexture(grassTileTopLeft)
        ->PushTexture(grassTileTopMiddle)
        ->PushTexture(flowerTileTopLeft)
        ->GetGameObject().GetComponent<TilemapRenderer>()
        ->SetSortingOrder(-1);

    tilemapGameObject_->GetComponent<Tilemap>()->tileSize = glm::vec2(8, 8);
    // tilemapGameObject_->GetComponent<Tilemap>()->SetDataBounds({0, 0}, {25, 25});
    auto bounds = tilemapGameObject_->GetComponent<Tilemap>()->GetBounds();
    tilemapGameObject_->GetTransform().SetPosition(glm::vec3(bounds.x * -0.5f, bounds.y * -0.5f, 0));
    // tilemapGameObject_->AddComponent<Pathfinder>();
    // tilemapGameObject_->SetTag("Pathfinder");
}

void PlayScene::SetupMiners()
{
    constexpr glm::vec2 enemyCellSize = glm::vec2(126, 39);
    minerSprite_ = CreateRef<Texture2D>("Assets/Spritesheets/Ball and Chain Bot/idle.png");
    auto& enemyIdleSpritesheet = minerSprite_;

    for (int i = 0; i < 3; ++i)
    {
        auto enemySpriteRenderer = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.f))
            ->AddComponent<SpriteRenderer>(SubTexture2D::CreateFromCoords(
                enemyIdleSpritesheet, glm::vec2(0, 1), enemyCellSize));
        
        enemySpriteRenderer->GetGameObject().SetTag("Miner");
        enemySpriteRenderer->SetNativeSize();
        // if (i)
        // {
        Ref<GameObject> arrow = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.25f));
        auto arrowSpriteRenderer = arrow->AddComponent<SpriteRenderer>(arrowSprite_);
        arrowSpriteRenderer->SetNativeSize();
        arrowSpriteRenderer->GetTransform().SetScale(
            arrowSpriteRenderer->GetTransform().GetScale()
            * glm::vec3(8, 8, 1));
        arrowSpriteRenderer->SetSortingOrder(50);
        arrow->SetActive(false);

        enemySpriteRenderer->GetGameObject().AddComponent<MinerController>()
            ->SetTilemap(tilemapGameObject_->GetComponent<Tilemap>())
            ->SetMineController(mineController_)
            ->SetArrowObject(arrow);
        // }

        enemySpriteRenderer->GetGameObject().AddComponent<BoxCollider2D>()->GetBounds().SetLocalExtents(glm::vec3(39.0f/126.0f, 1, 1) 
            * glm::vec3(ppiMultiplier_, 1));
        enemySpriteRenderer->GetTransform().SetScale(
            enemySpriteRenderer->GetTransform().GetScale()
            * glm::vec3(ppiMultiplier_, 1));
        enemySpriteRenderer->SetSortingOrder(10);
        enemySpriteRenderer->GetTransform().SetPosition(
            tilemapGameObject_
                ->GetComponent<Tilemap>()
                ->LocalToWorld(glm::ivec2(0)));
    }

}

void PlayScene::SetupWitches()
{
    constexpr glm::vec2 enemyCellSize = glm::vec2(32, 48);
    auto enemyIdleSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/Blue_witch/B_witch_idle.png");

    for (int i = 0; i < 2; ++i)
    {
        auto enemySpriteRenderer = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.f))
            ->AddComponent<SpriteRenderer>(SubTexture2D::CreateFromCoords(
                enemyIdleSpritesheet, glm::vec2(0, 0), enemyCellSize));

        // auto arrowSpriteRenderer = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(0.25f))
        //     ->AddComponent<SpriteRenderer>(arrowSprite_);

        enemySpriteRenderer->GetGameObject().SetTag("Witch");
        enemySpriteRenderer->SetNativeSize();

        Ref<GameObject> arrow = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.25f));
        auto arrowSpriteRenderer = arrow->AddComponent<SpriteRenderer>(arrowSprite_);
        arrowSpriteRenderer->SetNativeSize();
        arrowSpriteRenderer->GetTransform().SetScale(
            arrowSpriteRenderer->GetTransform().GetScale()
            * glm::vec3(8, 8, 1));
        arrowSpriteRenderer->SetSortingOrder(50);
        arrow->SetActive(false);

        auto controller = enemySpriteRenderer->GetGameObject().AddComponent<WitchController>();
        controller->SetTilemap(tilemapGameObject_->GetComponent<Tilemap>());
        controller->SetBounds({ 0, 11 }, { 24, 13 });
        controller->SetArrowObject(arrow);

        enemySpriteRenderer->GetGameObject().AddComponent<BoxCollider2D>()->GetBounds().SetLocalExtents(glm::vec3(5, 32.0f/48.0f * 5, 5) 
            * glm::vec3(ppiMultiplier_, 1));
        enemySpriteRenderer->GetTransform().SetScale(
            enemySpriteRenderer->GetTransform().GetScale()
            * glm::vec3(ppiMultiplier_ * glm::vec2(0.75f), 1));
        enemySpriteRenderer->SetSortingOrder(10);
        enemySpriteRenderer->GetTransform().SetPosition(
            tilemapGameObject_
                ->GetComponent<Tilemap>()
                ->LocalToWorld(glm::ivec2(5 + 12 * i, 12)));
    }

}

void PlayScene::CreateMines()
{
    // MineController game object
    auto gameObject = CreateGameObject();
    mineController_ = gameObject->AddComponent<MineController>();
    mineController_->SetRespawnTime(18.0f);

    for (auto oreObject : CreateOrePile(1, 12))
    {
        mineController_->AddObject(oreObject);
    }

    for (auto oreObject : CreateOrePile(3, 12))
    {
        mineController_->AddObject(oreObject);
    }

    for (auto oreObject : CreateOrePile(5, 12))
    {
        mineController_->AddObject(oreObject);
    }

    for (auto oreObject : CreateOrePile(23, 12))
    {
        mineController_->AddObject(oreObject);
    }

    for (auto oreObject : CreateOrePile(21, 12))
    {
        mineController_->AddObject(oreObject);
    }

    for (auto oreObject : CreateOrePile(19, 12))
    {
        mineController_->AddObject(oreObject);
    }

    // for (auto& oreObject : CreateOrePile(76, 36))
    // {
    //     mineController_->AddObject(oreObject);
    // }

}

void PlayScene::SetupDisplay()
{
    auto gameObject = CreateGameObject();
    enemyDisplay_ = gameObject->AddComponent<EnemyDisplay>();
    enemyDisplay_->Init(
        tilemapGameObject_->GetComponent<Tilemap>(),
        CreateRef<EnemyPool>([&]()
        {
            auto enemySpriteRenderer = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.f))
                ->AddComponent<SpriteRenderer>(SubTexture2D::CreateFromCoords(
                    minerSprite_, glm::vec2(0, 1), glm::vec2(126, 39)));
            
            enemySpriteRenderer->GetGameObject().SetTag("Miner");
            enemySpriteRenderer->SetNativeSize();
            // if (i)
            // {
            Ref<GameObject> arrow = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.25f));
            auto arrowSpriteRenderer = arrow->AddComponent<SpriteRenderer>(arrowSprite_);
            arrowSpriteRenderer->SetNativeSize();
            arrowSpriteRenderer->GetTransform().SetScale(
                arrowSpriteRenderer->GetTransform().GetScale()
                * glm::vec3(8, 8, 1));
            arrowSpriteRenderer->SetSortingOrder(50);
            arrow->SetActive(false);

            auto controller = enemySpriteRenderer->GetGameObject().AddComponent<MinerController>();
            controller->SetTilemap(tilemapGameObject_->GetComponent<Tilemap>())
                ->SetMineController(mineController_)
                ->SetArrowObject(arrow);
            // }

            SceneManager::Instance()->GetActiveScene()->UpdateRenderer(arrowSpriteRenderer.get());

            enemySpriteRenderer->GetGameObject().AddComponent<BoxCollider2D>()->GetBounds().SetLocalExtents(glm::vec3(39.0f/126.0f, 1, 1) 
                * glm::vec3(ppiMultiplier_, 1));
            enemySpriteRenderer->GetTransform().SetScale(
                enemySpriteRenderer->GetTransform().GetScale()
                * glm::vec3(ppiMultiplier_, 1));
            enemySpriteRenderer->SetSortingOrder(10);

            SceneManager::Instance()->GetActiveScene()->UpdateRenderer(enemySpriteRenderer.get());
            return controller;
        }),
        CreateRef<EnemyPool>([&]()
        {
            constexpr glm::vec2 enemyCellSize = glm::vec2(100, 64);
            auto enemySpriteRenderer = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.00f))
                ->AddComponent<SpriteRenderer>(SubTexture2D::CreateFromCoords(
                    knightSprite_, glm::vec2(0, 0), enemyCellSize));

            // auto arrowSpriteRenderer = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(0.25f))
            //     ->AddComponent<SpriteRenderer>(arrowSprite_);

            enemySpriteRenderer->GetGameObject().SetTag("Knight");
            enemySpriteRenderer->SetNativeSize();

            Ref<GameObject> arrow = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.25f));
            auto arrowSpriteRenderer = arrow->AddComponent<SpriteRenderer>(arrowSprite_);
            arrowSpriteRenderer->SetNativeSize();
            arrowSpriteRenderer->GetTransform().SetScale(
                arrowSpriteRenderer->GetTransform().GetScale()
                * glm::vec3(8, 8, 1));
            arrowSpriteRenderer->SetSortingOrder(50);
            arrow->SetActive(false);

            auto controller = enemySpriteRenderer->GetGameObject().AddComponent<KnightController>();
            controller->SetTilemap(tilemapGameObject_->GetComponent<Tilemap>());
            // controller->SetBounds({ 0, 11 }, { 24, 13 });
            controller->SetArrowObject(arrow);

            enemySpriteRenderer->GetGameObject().AddComponent<BoxCollider2D>()->GetBounds().SetLocalExtents(glm::vec3(64.0f/100.0f, 1, 1) 
                * glm::vec3(ppiMultiplier_, 1));
            enemySpriteRenderer->GetTransform().SetScale(
                enemySpriteRenderer->GetTransform().GetScale()
                * glm::vec3(ppiMultiplier_ * glm::vec2(0.35f), 1));
            enemySpriteRenderer->SetSortingOrder(10);
            enemySpriteRenderer->GetTransform().SetPosition(
                tilemapGameObject_
                    ->GetComponent<Tilemap>()
                    ->LocalToWorld(glm::ivec2(3 + i * 3, 3)));
            enemyDisplay_->AddKnightToPool(controller);
        })
    );
}

void PlayScene::SetupKnights()
{
    constexpr glm::vec2 enemyCellSize = glm::vec2(100, 64);
    knightSprite_ = CreateRef<Texture2D>(
        "Assets/Spritesheets/Idle_KG_1.png");
    auto& enemyIdleSpritesheet = knightSprite_;

    for (int i = 0; i < 3; ++i)
    {
        auto enemySpriteRenderer = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.00f))
            ->AddComponent<SpriteRenderer>(SubTexture2D::CreateFromCoords(
                knightSprite_, glm::vec2(0, 0), enemyCellSize));

        // auto arrowSpriteRenderer = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(0.25f))
        //     ->AddComponent<SpriteRenderer>(arrowSprite_);

        enemySpriteRenderer->GetGameObject().SetTag("Knight");
        enemySpriteRenderer->SetNativeSize();

        Ref<GameObject> arrow = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.25f));
        auto arrowSpriteRenderer = arrow->AddComponent<SpriteRenderer>(arrowSprite_);
        arrowSpriteRenderer->SetNativeSize();
        arrowSpriteRenderer->GetTransform().SetScale(
            arrowSpriteRenderer->GetTransform().GetScale()
            * glm::vec3(8, 8, 1));
        arrowSpriteRenderer->SetSortingOrder(50);
        arrow->SetActive(false);

        auto controller = enemySpriteRenderer->GetGameObject().AddComponent<KnightController>();
        controller->SetTilemap(tilemapGameObject_->GetComponent<Tilemap>());
        // controller->SetBounds({ 0, 11 }, { 24, 13 });
        controller->SetArrowObject(arrow);

        enemySpriteRenderer->GetGameObject().AddComponent<BoxCollider2D>()->GetBounds().SetLocalExtents(glm::vec3(64.0f/100.0f, 1, 1) 
            * glm::vec3(ppiMultiplier_, 1));
        enemySpriteRenderer->GetTransform().SetScale(
            enemySpriteRenderer->GetTransform().GetScale()
            * glm::vec3(ppiMultiplier_ * glm::vec2(0.35f), 1));
        enemySpriteRenderer->SetSortingOrder(10);
        enemySpriteRenderer->GetTransform().SetPosition(
            tilemapGameObject_
                ->GetComponent<Tilemap>()
                ->LocalToWorld(glm::ivec2(3 + i * 3, 3)));
        enemyDisplay_->AddKnightToPool(controller);
    }
}

std::vector<Ref<GameObject>> PlayScene::CreateOrePile(uint32_t x, uint32_t y)
{
    std::vector<Ref<GameObject>> orePileGameObjects;

    constexpr std::array<glm::vec3, 3> translations
    {
        {
            { 0, 0, 0 },
            { 2, -1.5, 0 },
            { -1.5f, -2, 0 }
        }
    };

    for (int i = 0; i < 3; ++i)
    {
        auto gameObject = CreateGameObject();
        gameObject->AddComponent<SpriteRenderer>(ironOreSprite_);
        gameObject->SetTag("Ore");

        gameObject->GetTransform().SetScale(glm::vec3(
            glm::vec2(16, 16) * ppiMultiplier_, 0));
        gameObject->GetTransform().SetPosition(
            tilemapGameObject_ 
                ->GetComponent<Tilemap>()
                ->LocalToWorld(x, y));
        gameObject->GetTransform().Translate(translations[i]);

        auto boxCollider = gameObject->AddComponent<BoxCollider2D>();
        boxCollider->GetBounds().SetLocalExtents(glm::vec3(ppiMultiplier_, 1));

        // auto triggerCallback = gameObject->AddComponent<TriggerCallback>();
        // triggerCallback->SetTagCondition("Miner");
        // triggerCallback->SetCallback([triggerCallback](Collider2D* other)
        // {
        //     other->enabled = false;
        //     triggerCallback->GetGameObject().SetActive(false);
        // });

        orePileGameObjects.push_back(gameObject);
    }

    return orePileGameObjects;
}
