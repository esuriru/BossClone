#include "MainLayer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#include "Physics/PhysicsComponent.h"

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

    // Register components
    coordinator->RegisterComponent<TransformComponent>();
    coordinator->RegisterComponent<SpriteRendererComponent>();
    coordinator->RegisterComponent<TileRendererComponent>();
    coordinator->RegisterComponent<TagComponent>();
    coordinator->RegisterComponent<TilemapComponent>();
    coordinator->RegisterComponent<RigidBody2DComponent>();
    coordinator->RegisterComponent<BoxCollider2DComponent>();
    coordinator->RegisterComponent<PlayerController2DComponent>();
    coordinator->RegisterComponent<RunningAnimationComponent>();

    spriteRenderSystem_ = coordinator->RegisterSystem<SpriteRenderSystem>();
    tilemapRenderSystem_ = coordinator->RegisterSystem<TilemapRenderSystem>();
    physicsSystem_ = coordinator->RegisterSystem<PhysicsSystem>();

    // NOTE - Don't need to update this tilemap system for now.
    physicsSystem_->tilemapSystem = coordinator->RegisterSystem<ActiveTilemapSystem>();
    playerSystem_ = coordinator->RegisterSystem<PlayerSystem>();
    playerSystem_->physicsSystem = physicsSystem_;
    playerSystem_->eventCallback = CC_BIND_EVENT_FUNC(MainLayer::OnEvent);

    runningAnimationSystem_ = coordinator->RegisterSystem<RunningAnimationSystem>();

    Signature spriteRenderSystemSignature;
    spriteRenderSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    spriteRenderSystemSignature.set(coordinator->GetComponentType<SpriteRendererComponent>());
    coordinator->SetSystemSignature<SpriteRenderSystem>(spriteRenderSystemSignature);

    Signature tileRenderSystemSignature;
    tileRenderSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    tileRenderSystemSignature.set(coordinator->GetComponentType<TilemapComponent>());
    coordinator->SetSystemSignature<TilemapRenderSystem>(tileRenderSystemSignature);

    Signature physicsSystemSignature;
    physicsSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    physicsSystemSignature.set(coordinator->GetComponentType<RigidBody2DComponent>());
    physicsSystemSignature.set(coordinator->GetComponentType<BoxCollider2DComponent>());
    coordinator->SetSystemSignature<PhysicsSystem>(physicsSystemSignature);

    Signature activeTilemapSystemSignature;
    activeTilemapSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    activeTilemapSystemSignature.set(coordinator->GetComponentType<TilemapComponent>());
    coordinator->SetSystemSignature<ActiveTilemapSystem>(activeTilemapSystemSignature);

    Signature playerSystemSignature;
    playerSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    playerSystemSignature.set(coordinator->GetComponentType<RigidBody2DComponent>());
    playerSystemSignature.set(coordinator->GetComponentType<PlayerController2DComponent>());
    coordinator->SetSystemSignature<PlayerSystem>(playerSystemSignature);

    Signature runningAnimationSystemSignature;
    runningAnimationSystemSignature.set(coordinator->GetComponentType<RunningAnimationComponent>());
    runningAnimationSystemSignature.set(coordinator->GetComponentType<SpriteRendererComponent>());
    coordinator->SetSystemSignature<RunningAnimationSystem>(runningAnimationSystemSignature);

    constexpr glm::vec2 pixelAdventureTileSize = glm::vec2(16 ,16);
    auto grassTileTopLeft = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 10), pixelAdventureTileSize);
    auto grassTileTopMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 10), pixelAdventureTileSize);
    auto grassTileTopRight = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 10), pixelAdventureTileSize);
    auto grassTileMiddleLeft = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 9), pixelAdventureTileSize);
    auto grassTileMiddleMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 9), pixelAdventureTileSize);
    auto grassTileMiddleRight = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 9), pixelAdventureTileSize);
    auto grassTileBottomLeft = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 8), pixelAdventureTileSize);
    auto grassTileBottomMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 8),pixelAdventureTileSize);
    auto grassTileBottomRight = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 8), pixelAdventureTileSize);
    auto platformStart = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(17, 9), pixelAdventureTileSize);
    auto platformEnd = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(19, 9), pixelAdventureTileSize);

    auto tilemapEntity = coordinator->CreateEntity();

    TilemapComponent tilemapComponent("Assets/Maps/TestMap.csv", "Assets/Maps/TestMapTypes.csv");
    tilemapComponent.SubTextureMap[1] = grassTileTopLeft;
    tilemapComponent.SubTextureMap[2] = grassTileTopMiddle;
    tilemapComponent.SubTextureMap[3] = grassTileTopRight;
    tilemapComponent.SubTextureMap[4] = grassTileMiddleLeft;
    tilemapComponent.SubTextureMap[5] = grassTileMiddleMiddle;
    tilemapComponent.SubTextureMap[6] = grassTileMiddleRight;
    tilemapComponent.SubTextureMap[7] = grassTileBottomLeft;
    tilemapComponent.SubTextureMap[8] = grassTileBottomMiddle;
    tilemapComponent.SubTextureMap[9] = grassTileBottomRight;
    tilemapComponent.SubTextureMap[10] = platformStart;
    tilemapComponent.SubTextureMap[11] = platformEnd;

    coordinator->AddComponent(tilemapEntity, TransformComponent {
        // glm::vec3(TilemapData::TILEMAP_MAX_X_LENGTH * -0.5f * tilemapComponent.TileSize.x, TilemapData::TILEMAP_MAX_Y_LENGTH * -0.5f * tilemapComponent.TileSize.y, 0),
        glm::vec3(TilemapData::TILEMAP_MAX_X_LENGTH * -0.5f * tilemapComponent.TileSize.x, -70, 0),

    });

    coordinator->AddComponent(tilemapEntity, tilemapComponent);

    auto playerEntity = coordinator->CreateEntity();

    coordinator->AddComponent(playerEntity, TransformComponent{
        glm::vec3(), 
        glm::vec3(), 
        glm::vec3(32, 32, 1), 
    });
    auto rigidbody = RigidBody2DComponent();
    rigidbody.SetMass(3.f);
    coordinator->AddComponent(playerEntity, rigidbody);
    coordinator->AddComponent(playerEntity, BoxCollider2DComponent(
        glm::vec2(0, -3.f),
        glm::vec2(13.f, 13.f)
    ));

    playerIdleSpritesheet_ = CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Main Characters/Ninja Frog/Idle (32x32).png");
    auto playerSpriteRendererComponent = SpriteRendererComponent();
    playerSpriteRendererComponent.Texture = (SubTexture2D::CreateFromCoords(playerIdleSpritesheet_, glm::vec2(1, 1), glm::vec2(32, 32)));
    coordinator->AddComponent(playerEntity, playerSpriteRendererComponent);
    coordinator->AddComponent(playerEntity, PlayerController2DComponent());
    auto runningAnimationComponent = RunningAnimationComponent();

    auto playerRunSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Main Characters/Ninja Frog/Run (32x32).png");
    auto& runningAnimation = runningAnimationComponent.Animation;
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(1 , 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(2 , 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(3 , 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(4 , 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(5 , 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(6 , 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(7 , 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(8 , 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(9 , 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(10, 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(11, 1), glm::vec2(32, 32)));
    runningAnimation.SpriteTextures.push_back(SubTexture2D::CreateFromCoords(playerRunSpritesheet, glm::vec2(12, 1), glm::vec2(32, 32)));

    for (size_t i = 0; i < runningAnimation.SpriteTextures.size(); ++i)
    {
        runningAnimation.AnimationIndices.emplace_back(i);
    }

    runningAnimation.FramesBetweenTransition = 120;

    coordinator->AddComponent(playerEntity, runningAnimationComponent);
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
    physicsSystem_->Update(ts);
    playerSystem_->Update(ts);

    RenderCommand::SetClearColour(glm::vec4(135.f/255.f, 206.f/255.f, 250.f/255.f, 1.0f));
    RenderCommand::Clear();

    Renderer2D::BeginScene(cameraController_.GetCamera());

    runningAnimationSystem_->Update(ts);
    tilemapRenderSystem_->Update(ts);
    spriteRenderSystem_->Update(ts);

    Renderer2D::EndScene();
}

auto MainLayer::OnEvent(Event &e) -> void 
{
    cameraController_.OnEvent(e);
    runningAnimationSystem_->OnEvent(e);    
}
