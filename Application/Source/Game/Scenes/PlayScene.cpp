#include "PlayScene.h"
#include "EC/Components/SpriteRenderer.h"
#include "EC/Components/Tilemap.h"
#include "EC/Components/TilemapRenderer.h"
#include "EC/Components/EnemyController.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

PlayScene::PlayScene()
    : Scene("PlayScene")
{

    auto tilemapSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/Tilemap/TX Tileset Grass.png");

    constexpr glm::vec2 tilemapTileSize = glm::vec2(32, 32);
    constexpr glm::vec2 ppiMultiplier = glm::vec2(8, 8) / 32.0f;

    auto grassTileTopLeft = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(1, 7), tilemapTileSize);
    auto grassTileTopMiddle = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(2, 7), tilemapTileSize);
    auto flowerTileTopLeft = SubTexture2D::CreateFromCoords(tilemapSpritesheet, glm::vec2(4, 7), tilemapTileSize);

    
    auto tilemapObject = CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(1.f));
        tilemapObject->AddComponent<Tilemap>("Assets/Maps/SimulationMap.csv", "Assets/Maps/SimulationMapTypes.csv")
        ->PushTexture(grassTileTopLeft)
        ->PushTexture(grassTileTopMiddle)
        ->PushTexture(flowerTileTopLeft)
        ->GetGameObject().GetComponent<TilemapRenderer>()
        ->SetSortingOrder(1);

    tilemapObject->GetComponent<Tilemap>()->tileSize = glm::vec2(8, 8);
    auto bounds = tilemapObject->GetComponent<Tilemap>()->GetBounds();
    tilemapObject->GetTransform().SetPosition(glm::vec3(bounds.x * -0.5f, bounds.y * -0.5f, 0));

    constexpr glm::vec2 enemyCellSize = glm::vec2(126, 39);
    auto enemyIdleSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/Ball and Chain Bot/idle.png");

    auto enemySpriteRenderer = CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(1.f))
        ->AddComponent<SpriteRenderer>(SubTexture2D::CreateFromCoords(
            enemyIdleSpritesheet, glm::vec2(0, 1), enemyCellSize));
    enemySpriteRenderer->SetNativeSize();
    enemySpriteRenderer->GetTransform().SetScale(
        enemySpriteRenderer->GetTransform().GetScale()
        * glm::vec3(ppiMultiplier, 0));
    enemySpriteRenderer->SetSortingOrder(10);
    enemySpriteRenderer->GetGameObject().AddComponent<EnemyController>()
        ->SetTilemap(tilemapObject->GetComponent<Tilemap>());

    auto localPosition = tilemapObject->GetComponent<Tilemap>()
        ->WorldToLocal(enemySpriteRenderer->GetTransform()
        .GetPosition());

    CC_TRACE("Enemy tile position: x: ", localPosition.first, ", y: ", localPosition.second);

}
