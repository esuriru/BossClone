#include "MainLayer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#include "Physics/PhysicsComponent.h"

#include "Utils/Input.h"
#include "Utils/Util.h"

MainLayer::MainLayer()
    : Layer("Main")
    , cameraController_(1280.0f/ 720.0f)
{
    this->nareLogoTexture_ = CreateRef<Texture2D>("Assets/Images/Nare Logo.png");
    terrainSpritesheet_ = CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Terrain/Terrain (16x16).png");
    playerSpritesheet_ = CreateRef<Texture2D>("Assets/Spritesheets/HoodedCharacter/AnimationSheet_Character.png");

    static Coordinator* coordinator = Coordinator::Instance();

    coordinator->Init();

    // TODO - Consider using pragma region.

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
    coordinator->RegisterComponent<SwingingAnimationComponent>();
    coordinator->RegisterComponent<WeaponComponent>();
    coordinator->RegisterComponent<OwnedByComponent>();
    coordinator->RegisterComponent<InventoryComponent>();
    coordinator->RegisterComponent<PhysicsQuadtreeComponent>();
    coordinator->RegisterComponent<HealthComponent>();
    coordinator->RegisterComponent<AffectedByAnimationComponent>();
    coordinator->RegisterComponent<WeaponAffectedByAnimationComponent>();

    // System registry

    spriteRenderSystem_ = coordinator->RegisterSystem<SpriteRenderSystem>();
    tilemapRenderSystem_ = coordinator->RegisterSystem<TilemapRenderSystem>();
    physicsSystem_ = coordinator->RegisterSystem<PhysicsSystem>();

    // NOTE - Don't need to update this tilemap system for now.
    physicsSystem_->tilemapSystem = coordinator->RegisterSystem<ActiveTilemapSystem>();
    playerSystem_ = coordinator->RegisterSystem<PlayerSystem>();
    playerSystem_->physicsSystem = physicsSystem_;

    runningAnimationSystem_ = coordinator->RegisterSystem<RunningAnimationSystem>();
    swingingAnimationSystem_ = coordinator->RegisterSystem<SwingingAnimationSystem>();

    weaponSystem_ = coordinator->RegisterSystem<WeaponSystem>();
    weaponSystem_->eventCallback = CC_BIND_EVENT_FUNC(MainLayer::OnEvent);

    damageableSystem_ = coordinator->RegisterSystem<DamageableSystem>();

    playerAffectedByAnimationSystem_ = coordinator->RegisterSystem<PlayerAffectedByAnimationSystem>();
    weaponAffectedByAnimationSystem_ = coordinator->RegisterSystem<WeaponAffectedByAnimationSystem>();

    // Set event callbacks for most of the systems.
    playerSystem_->eventCallback = physicsSystem_->tilemapSystem->eventCallback =
        runningAnimationSystem_->eventCallback = weaponSystem_->eventCallback =
            swingingAnimationSystem_->eventCallback = 
                weaponAffectedByAnimationSystem_->eventCallback =
                    playerAffectedByAnimationSystem_->eventCallback =
                        CC_BIND_EVENT_FUNC(MainLayer::OnEvent);

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
    physicsSystemSignature.set(coordinator->GetComponentType<PhysicsQuadtreeComponent>());
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

    Signature swingingAnimationSystemSignature;
    swingingAnimationSystemSignature.set(coordinator->GetComponentType<SwingingAnimationComponent>());
    swingingAnimationSystemSignature.set(coordinator->GetComponentType<SpriteRendererComponent>());
    coordinator->SetSystemSignature<SwingingAnimationSystem>(swingingAnimationSystemSignature);

    Signature weaponSystemSignature;
    weaponSystemSignature.set(coordinator->GetComponentType<WeaponComponent>());
    weaponSystemSignature.set(coordinator->GetComponentType<OwnedByComponent>());
    coordinator->SetSystemSignature<WeaponSystem>(weaponSystemSignature);

    Signature damageableSystemSignature;
    damageableSystemSignature.set(coordinator->GetComponentType<HealthComponent>());
    damageableSystemSignature.set(coordinator->GetComponentType<PhysicsQuadtreeComponent>());
    coordinator->SetSystemSignature<DamageableSystem>(damageableSystemSignature);

    Signature weaponAffectedByAnimationSystemSignature;
    weaponAffectedByAnimationSystemSignature.set(coordinator->GetComponentType<WeaponComponent>());
    weaponAffectedByAnimationSystemSignature.set(coordinator->GetComponentType<WeaponAffectedByAnimationComponent>());
    weaponAffectedByAnimationSystemSignature.set(coordinator->GetComponentType<PhysicsQuadtreeComponent>());
    coordinator->SetSystemSignature<WeaponAffectedByAnimationSystem>(weaponAffectedByAnimationSystemSignature);

    Signature playerAffectedByAnimationSystemSignature;
    playerAffectedByAnimationSystemSignature.set(coordinator->GetComponentType<PlayerController2DComponent>());
    playerAffectedByAnimationSystemSignature.set(coordinator->GetComponentType<AffectedByAnimationComponent>());
    playerAffectedByAnimationSystemSignature.set(coordinator->GetComponentType<InventoryComponent>());
    coordinator->SetSystemSignature<PlayerAffectedByAnimationSystem>(playerAffectedByAnimationSystemSignature);

    // TODO - Fix tilemap bleeding
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
        glm::vec2(0, 0),
        glm::vec2(8.5f, 16.f)
    ));

    constexpr glm::vec2 playerTextureSize = glm::vec2(32.f, 32.f);

    auto playerSpriteRendererComponent = SpriteRendererComponent();
    playerSpriteRendererComponent.Texture =
        (SubTexture2D::CreateFromCoords(playerSpritesheet_, glm::vec2(0, 8), playerTextureSize));
    runningAnimationSystem_->SetOriginalTexture(playerSpriteRendererComponent.Texture, playerEntity);
    swingingAnimationSystem_->SetOriginalTexture(playerSpriteRendererComponent.Texture, playerEntity);

    coordinator->AddComponent(playerEntity, playerSpriteRendererComponent);

    auto playerControllerComponent = PlayerController2DComponent();
    playerControllerComponent.ActiveMeleeWeaponIndices = { 5 };
    coordinator->AddComponent(playerEntity, playerControllerComponent);

    coordinator->AddComponent(playerEntity, AffectedByAnimationComponent(Animation::AnimationType::Swinging));

    // Running animation
    RunningAnimationComponent runningAnimationComponent;

    auto& runningAnimation = runningAnimationComponent.Animation;
    for (int i = 0; i < 4; ++i)
    {
        runningAnimation.SpriteTextures.emplace_back(
            SubTexture2D::CreateFromCoords(playerSpritesheet_, glm::vec2(i , 6), playerTextureSize));
    }

    for (size_t i = 0; i < runningAnimation.SpriteTextures.size(); ++i)
    {
        runningAnimation.AnimationIndices.emplace_back(i);
    }

    runningAnimation.FramesBetweenTransition = 240;

    coordinator->AddComponent(playerEntity, runningAnimationComponent);

    // Swinging animation
    SwingingAnimationComponent swingingAnimationComponent;
    auto& swingingAnimation = swingingAnimationComponent.Animation;

    for (int i = 0; i < 8; ++i)
    {
        swingingAnimation.SpriteTextures.emplace_back(
            SubTexture2D::CreateFromCoords(playerSpritesheet_, glm::vec2(i , 0), playerTextureSize));
    }

    for (size_t i = 0; i < swingingAnimation.SpriteTextures.size(); ++i)
    {
        swingingAnimation.AnimationIndices.emplace_back(i);
    }

    swingingAnimation.FramesBetweenTransition = 240;

    coordinator->AddComponent(playerEntity, swingingAnimationComponent);

    auto inventoryComponent = InventoryComponent();

    auto meleeWeaponEntity = coordinator->CreateEntity();

    WeaponComponent meleeWeaponComponent;
    meleeWeaponComponent.Behaviour = WeaponSystem::MeleeBehaviour;
    meleeWeaponComponent.HandOffset = { 11, -2 };
    meleeWeaponComponent.Damage = 5.0f;
    coordinator->AddComponent(meleeWeaponEntity, meleeWeaponComponent);
    coordinator->AddComponent(meleeWeaponEntity, OwnedByComponent(playerEntity));

    WeaponAffectedByAnimationComponent meleeWeaponAffectedByAnimationComponent;

    // When the index is part of the weapons 'active index', enable the collider for melee weapons.
    meleeWeaponAffectedByAnimationComponent.AnimationBehaviour =
        [](Entity weaponEntity, AnimationSpriteChangeEvent& ascEvent,
            const std::set<size_t>& activeIndices)
        {
            bool indexIsActive = activeIndices.find(ascEvent.GetSpriteIndex()) != activeIndices.end();
            coordinator->GetComponent<PhysicsQuadtreeComponent>(weaponEntity).Active = indexIsActive;
        };

    coordinator->AddComponent(meleeWeaponEntity, meleeWeaponAffectedByAnimationComponent);

    inventoryComponent.Items.emplace_back(meleeWeaponEntity);
    inventoryComponent.CurrentlyHolding = meleeWeaponEntity;

    coordinator->AddComponent(meleeWeaponEntity, TransformComponent(glm::vec3(0.f, 0, 0),
        glm::vec3(0), glm::vec3(6, 12, 1)));
    coordinator->AddComponent(meleeWeaponEntity, SpriteRendererComponent(glm::vec4(1.0f, 0, 0, 1.0f)));
    RigidBody2DComponent meleeWeaponRigidbody;
    meleeWeaponRigidbody.BodyType = Physics::RigidBodyType::Static;

    coordinator->AddComponent(meleeWeaponEntity, meleeWeaponRigidbody); 

    auto meleeBoxCollider = BoxCollider2DComponent();
    meleeBoxCollider.Extents = glm::vec2(3, 6);
    coordinator->AddComponent(meleeWeaponEntity, meleeBoxCollider); 
    coordinator->AddComponent(meleeWeaponEntity, PhysicsQuadtreeComponent()); 

    coordinator->AddComponent(playerEntity, inventoryComponent);
    coordinator->AddComponent(playerEntity, PhysicsQuadtreeComponent());

    auto testPhysicsEntity = coordinator->CreateEntity();
    coordinator->AddComponent(testPhysicsEntity, TransformComponent(glm::vec3(20, 0, 0),
       glm::vec3(0), glm::vec3(12, 12, 1)));
    coordinator->AddComponent(testPhysicsEntity, SpriteRendererComponent()); 

    auto testPhysicsEntityCollider = BoxCollider2DComponent();
    testPhysicsEntityCollider.Extents = glm::vec2(6, 6);
    coordinator->AddComponent(testPhysicsEntity, testPhysicsEntityCollider); 
    coordinator->AddComponent(testPhysicsEntity, RigidBody2DComponent());
    coordinator->AddComponent(testPhysicsEntity, PhysicsQuadtreeComponent());
    coordinator->AddComponent(testPhysicsEntity, HealthComponent(100));

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
    weaponSystem_->Update(ts);
    damageableSystem_->Update(ts);
    physicsSystem_->Update(ts);
    playerSystem_->Update(ts);

    constexpr glm::vec4 background_colour = glm::vec4(135.f, 206.f, 250.f, 1.0f);
    RenderCommand::SetClearColour(Utility::Colour32BitConvert(background_colour));
    RenderCommand::Clear();

    Renderer2D::BeginScene(cameraController_.GetCamera());

    runningAnimationSystem_->Update(ts);
    swingingAnimationSystem_->Update(ts);

    tilemapRenderSystem_->Update(ts);
    spriteRenderSystem_->Update(ts);

    Renderer2D::EndScene();
}

auto MainLayer::OnEvent(Event &e) -> void 
{
    // Process all the logic stuff first before rendering.
    weaponSystem_->OnEvent(e);

    // All the render stuff should be after.
    cameraController_.OnEvent(e);
    damageableSystem_->OnEvent(e);

    // Run the animations
    runningAnimationSystem_->OnEvent(e);    
    swingingAnimationSystem_->OnEvent(e);

    // Animation sprite change events are now called, now setup callbacks
    playerAffectedByAnimationSystem_->OnEvent(e);
    weaponAffectedByAnimationSystem_->OnEvent(e);
}
