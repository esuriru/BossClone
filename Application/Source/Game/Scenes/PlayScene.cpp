#include "PlayScene.h"
#include "EC/Components/SpriteRenderer.h"
#include "EC/Components/Tilemap.h"
#include "EC/Components/TilemapRenderer.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

PlayScene::PlayScene()
    : Scene("PlayScene")
{
    nareLogoTexture_ = CreateRef<Texture2D>("Assets/Images/Nare Logo.png");

    CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(100, 100, 1.f))
        ->AddComponent<SpriteRenderer>(nareLogoTexture_)
        ->SetSortingOrder(1);

    terrainSpritesheet_ = CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Terrain/Terrain (16x16).png");

    constexpr glm::vec2 pixelAdventureTileSize = glm::vec2(16 ,16);
    auto grassTileTopLeft       = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 10), pixelAdventureTileSize);
    auto grassTileTopMiddle     = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 10), pixelAdventureTileSize);
    auto grassTileTopRight      = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 10), pixelAdventureTileSize);
    auto grassTileMiddleLeft    = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 9 ), pixelAdventureTileSize);
    auto grassTileMiddleMiddle  = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 9 ), pixelAdventureTileSize);
    auto grassTileMiddleRight   = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 9 ), pixelAdventureTileSize);
    auto grassTileBottomLeft    = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 8 ), pixelAdventureTileSize);
    auto grassTileBottomMiddle  = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 8 ), pixelAdventureTileSize);
    auto grassTileBottomRight   = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 8 ), pixelAdventureTileSize);
    auto platformStart          = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(17, 9), pixelAdventureTileSize);
    auto platformEnd            = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(19, 9), pixelAdventureTileSize);
    auto goldBlockStart         = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(17, 2), pixelAdventureTileSize);
    auto goldBlockMiddle        = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(18, 2), pixelAdventureTileSize);
    auto goldBlockEnd           = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(19, 2), pixelAdventureTileSize);
    
    auto tilemapObject = CreateGameObject(glm::vec3(), glm::identity<glm::quat>(), glm::vec3(1.f));
        tilemapObject->AddComponent<Tilemap>("Assets/Maps/TestMap.csv", "Assets/Maps/TestMapTypes.csv")
        ->PushTexture(grassTileTopLeft)
        ->PushTexture(grassTileTopMiddle)
        ->PushTexture(grassTileTopRight)
        ->PushTexture(grassTileMiddleLeft)
        ->PushTexture(grassTileMiddleMiddle)
        ->PushTexture(grassTileMiddleRight)
        ->PushTexture(grassTileBottomLeft)
        ->PushTexture(grassTileBottomMiddle)
        ->PushTexture(grassTileBottomRight)
        ->PushTexture(platformStart)
        ->PushTexture(platformEnd)
        ->PushTexture(goldBlockStart)
        ->PushTexture(goldBlockMiddle)
        ->PushTexture(goldBlockEnd)
        ->GetGameObject().GetComponent<TilemapRenderer>()
        ->SetSortingOrder(3);

    auto bounds = tilemapObject->GetComponent<Tilemap>()->GetBounds();
    tilemapObject->GetTransform().SetPosition(glm::vec3(bounds.x * -0.5f, bounds.y * -0.5f, 0));
}
