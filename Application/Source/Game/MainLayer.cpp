#include "MainLayer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#include "Utils/Input.h"

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
    coordinator->RegisterComponent<TilemapComponent>();

    spriteRenderSystem_ = coordinator->RegisterSystem<SpriteRenderSystem>();
    tilemapRenderSystem_ = coordinator->RegisterSystem<TilemapRenderSystem>();

    Signature spriteRenderSystemSignature;
    spriteRenderSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    spriteRenderSystemSignature.set(coordinator->GetComponentType<SpriteRendererComponent>());
    coordinator->SetSystemSignature<SpriteRenderSystem>(spriteRenderSystemSignature);

    Signature tileRenderSystemSignature;
    tileRenderSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    tileRenderSystemSignature.set(coordinator->GetComponentType<TilemapComponent>());
    coordinator->SetSystemSignature<TilemapRenderSystem>(tileRenderSystemSignature);

    // for (int i = 0; i < 5; ++i)
    // {
    //     auto entity = coordinator->CreateEntity();  
    //     coordinator->AddComponent(entity, TransformComponent {
    //         glm::vec3(10.f + 1.f * i, 0, 0),
    //     });

    //     coordinator->AddComponent(entity, SpriteRendererComponent{
    //         glm::vec4(0.8f, 0.f, 0.f, 1.0f)
    //     });
    // }

    auto grassTileTopLeft = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 10), glm::vec2(16, 16));
    auto grassTileTopMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 10), glm::vec2(16, 16));
    auto grassTileTopRight = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 10), glm::vec2(16, 16));
    auto grassTileMiddleLeft = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 9), glm::vec2(16, 16));
    auto grassTileMiddleMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 9), glm::vec2(16, 16));
    auto grassTileMiddleRight = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 9), glm::vec2(16, 16));
    auto grassTileBottomLeft = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 8), glm::vec2(16, 16));
    auto grassTileBottomMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 8), glm::vec2(16, 16));
    auto grassTileBottomRight = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 8), glm::vec2(16, 16));

    auto tilemapEntity = coordinator->CreateEntity();

    coordinator->AddComponent(tilemapEntity, TransformComponent {
        glm::vec3(0.f),
    });

    TilemapComponent tilemapComponent("Assets/Maps/TestMap.csv");
    tilemapComponent.SubTextureMap[1] = grassTileTopLeft;
    tilemapComponent.SubTextureMap[2] = grassTileTopMiddle;
    tilemapComponent.SubTextureMap[3] = grassTileTopRight;
    tilemapComponent.SubTextureMap[4] = grassTileMiddleLeft;
    tilemapComponent.SubTextureMap[5] = grassTileMiddleMiddle;
    tilemapComponent.SubTextureMap[6] = grassTileMiddleRight;
    tilemapComponent.SubTextureMap[7] = grassTileBottomLeft;
    tilemapComponent.SubTextureMap[8] = grassTileBottomMiddle;
    tilemapComponent.SubTextureMap[9] = grassTileBottomRight;

    coordinator->AddComponent(tilemapEntity, tilemapComponent);
}

auto MainLayer::OnAttach() -> void 
{
}

auto MainLayer::OnDetach() -> void 
{
    Input::Destroy();
    Coordinator::Destroy();
}

auto MainLayer::OnUpdate(Timestep ts) -> void 
{
    cameraController_.OnUpdate(ts);

    RenderCommand::SetClearColour(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    RenderCommand::Clear();

    Renderer2D::BeginScene(cameraController_.GetCamera());

    spriteRenderSystem_->Update(ts);
    tilemapRenderSystem_->Update(ts);
    Renderer2D::EndScene();
}

auto MainLayer::OnEvent(Event &e) -> void 
{
    cameraController_.OnEvent(e);
}
