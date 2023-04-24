#include "MainLayer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

MainLayer::MainLayer()
    : Layer("Main")
    , cameraController_(1280.0f/ 720.0f)
{
    this->nareLogoTexture_ = CreateRef<Texture2D>("Assets/Images/Nare Logo.png");
    terrainSpritesheet_ = CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Terrain/Terrain (16x16).png");

    // Register components
    static Coordinator* coordinator = Coordinator::Instance();

    coordinator->Init();

    coordinator->RegisterComponent<TransformComponent>();
    coordinator->RegisterComponent<SpriteRendererComponent>();
    coordinator->RegisterComponent<TileRendererComponent>();
    coordinator->RegisterComponent<TagComponent>();

    spriteRenderSystem_ = coordinator->RegisterSystem<SpriteRenderSystem>();
    tileRenderSystem_ = coordinator->RegisterSystem<TileRenderSystem>();

    Signature spriteRenderSystemSignature;
    spriteRenderSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    spriteRenderSystemSignature.set(coordinator->GetComponentType<SpriteRendererComponent>());
    coordinator->SetSystemSignature<SpriteRenderSystem>(spriteRenderSystemSignature);

    Signature tileRenderSystemSignature;
    tileRenderSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    tileRenderSystemSignature.set(coordinator->GetComponentType<TileRendererComponent>());
    coordinator->SetSystemSignature<TileRenderSystem>(tileRenderSystemSignature);

    for (int i = 0; i < 5; ++i)
    {
        auto entity = coordinator->CreateEntity();  
        coordinator->AddComponent(entity, TransformComponent {
            glm::vec3(10.f + 1.f * i, 0, 0),
        });

        coordinator->AddComponent(entity, SpriteRendererComponent{
            glm::vec4(0.8f, 0.f, 0.f, 1.0f)
        });
    }

    auto grassTileTopLeft = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 10), glm::vec2(16, 16));
    auto grassTileTopMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 10), glm::vec2(16, 16));
    auto grassTileTopRight = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 10), glm::vec2(16, 16));
    auto grassTileMiddleLeft = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 9), glm::vec2(16, 16));
    auto grassTileMiddleMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 9), glm::vec2(16, 16));
    auto grassTileMiddleRight = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 9), glm::vec2(16, 16));

    testTilemap_ = CreateRef<Tilemap>();
    testTilemap_->ImportTilemapCSV("Assets/Maps/TestMap.csv");

    // Grass/dirt tiles
    testTilemap_->SetSubTexture(1, grassTileTopLeft);
    testTilemap_->SetSubTexture(2, grassTileTopMiddle);
    testTilemap_->SetSubTexture(3, grassTileTopRight);
    testTilemap_->SetSubTexture(4, grassTileMiddleLeft);
    testTilemap_->SetSubTexture(5, grassTileMiddleMiddle);
    testTilemap_->SetSubTexture(6, grassTileMiddleRight);

    testTilemap_->GenerateEntities();

}

auto MainLayer::OnAttach() -> void 
{
}

auto MainLayer::OnDetach() -> void 
{
}

auto MainLayer::OnUpdate(Timestep ts) -> void 
{
    cameraController_.OnUpdate(ts);

    RenderCommand::SetClearColour(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    RenderCommand::Clear();

    Renderer2D::BeginScene(cameraController_.GetCamera());

    spriteRenderSystem_->Update(ts);
    tileRenderSystem_->Update(ts);
    Renderer2D::EndScene();
}

auto MainLayer::OnEvent(Event &e) -> void 
{
    cameraController_.OnEvent(e);
}
