#include "PlayScene.h"

#include "EC/Components/SpriteRenderer.h"
#include "EC/Components/TriggerCallback.h"
#include "EC/Components/Tilemap.h"
#include "EC/Components/TilemapRenderer.h"
#include "EC/Components/EnemyController.h"
#include "EC/Components/BoxCollider2D.h"
#include "EC/Components/Pathfinder.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <array>

PlayScene::PlayScene()
    : Scene("PlayScene")
    , ppiMultiplier_(glm::vec2(8, 8) / 32.0f)
    , ironOreSprite_(CreateRef<Texture2D>("Assets/Images/Ore2.png"))
{
    SetupTilemap();
    CreateMines();
    SetupMiners();
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
    auto enemyIdleSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/Ball and Chain Bot/idle.png");

    for (int i = 0; i < 2; ++i)
    {
        auto enemySpriteRenderer = CreateGameObject(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1.f))
            ->AddComponent<SpriteRenderer>(SubTexture2D::CreateFromCoords(
                enemyIdleSpritesheet, glm::vec2(0, 1), enemyCellSize));
        
        enemySpriteRenderer->GetGameObject().SetTag("Miner");
        enemySpriteRenderer->SetNativeSize();
        // if (i)
        // {
            enemySpriteRenderer->GetGameObject().AddComponent<EnemyController>()
                ->SetTilemap(tilemapGameObject_->GetComponent<Tilemap>())
                ->SetMineController(mineController_);
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

void PlayScene::CreateMines()
{
    // MineController game object
    auto gameObject = CreateGameObject();
    mineController_ = gameObject->AddComponent<MineController>();
    mineController_->SetRespawnTime(18.0f);

    for (auto oreObject : CreateOrePile(5, 5))
    {
        mineController_->AddObject(oreObject);
    }

    for (auto oreObject : CreateOrePile(7, 5))
    {
        mineController_->AddObject(oreObject);
    }

    for (auto oreObject : CreateOrePile(9, 5))
    {
        mineController_->AddObject(oreObject);
    }

    // for (auto& oreObject : CreateOrePile(76, 36))
    // {
    //     mineController_->AddObject(oreObject);
    // }

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
