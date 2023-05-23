#include "MainLayer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#include "Physics/PhysicsComponent.h"

#include "Utils/Input.h"
#include "Utils/Util.h"

#include <examples/imgui_impl_opengl3.h>

#include "imgui.h"

Ref<Texture2D> MainLayer::TransparentItemSpritesheet;
MainLayer::MainLayer()
    : Layer("Main")
    , cameraController_(1280.0f/ 720.0f)
{
    this->nareLogoTexture_ = CreateRef<Texture2D>("Assets/Images/Nare Logo.png");
    terrainSpritesheet_ = CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Terrain/Terrain (16x16).png");
    playerSpritesheet_ = CreateRef<Texture2D>("Assets/Spritesheets/HoodedCharacter/AnimationSheet_Character.png");
    TransparentItemSpritesheet = CreateRef<Texture2D>("Assets/Spritesheets/ShikashiFantasyIconPackV2/TIcons.png");
    blueBackground_ = CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Background/Blue.png");
    brownBackground_ = CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Background/Brown.png");
    spikeTexture_ = CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Traps/Spikes/Idle.png");

    static Coordinator* coordinator = Coordinator::Instance();
    coordinator->Init();
    coordinator->SetEventCallback(CC_BIND_EVENT_FUNC(MainLayer::OnEvent));

    // TODO - Consider using pragma region.

#pragma region COMPONENT_REGISTRY
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
    coordinator->RegisterComponent<ItemComponent>();
    coordinator->RegisterComponent<PickupComponent>();
    coordinator->RegisterComponent<WeaponAffectedByPickupComponent>();
    coordinator->RegisterComponent<BreakableComponent>();
    coordinator->RegisterComponent<SpikeComponent>();

#pragma endregion
#pragma region SYSTEM_REGISTRY
    spriteRenderSystem_  = coordinator->RegisterSystem<SpriteRenderSystem>();
    tilemapRenderSystem_ = coordinator->RegisterSystem<TilemapRenderSystem>();
    physicsSystem_       = coordinator->RegisterSystem<PhysicsSystem>();

      // NOTE - Don't need to update this tilemap system for now.
    physicsSystem_->tilemapSystem = coordinator->RegisterSystem<ActiveTilemapSystem>();
    playerSystem_                 = coordinator->RegisterSystem<PlayerSystem>();
    playerSystem_->physicsSystem  = physicsSystem_;

    runningAnimationSystem_           = coordinator->RegisterSystem<RunningAnimationSystem>();
    swingingAnimationSystem_          = coordinator->RegisterSystem<SwingingAnimationSystem>();
    weaponSystem_                     = coordinator->RegisterSystem<WeaponSystem>();
    damageableSystem_                 = coordinator->RegisterSystem<DamageableSystem>();
    playerAffectedByAnimationSystem_  = coordinator->RegisterSystem<PlayerAffectedByAnimationSystem>();
    weaponAffectedByAnimationSystem_  = coordinator->RegisterSystem<WeaponAffectedByAnimationSystem>();
    inventoryGUISystem_               = coordinator->RegisterSystem<InventoryGUISystem>();
    inventoryGUISystem_->helperSystem = coordinator->RegisterSystem<InventoryGUIHelperSystem>();
    pickupSystem_                     = coordinator->RegisterSystem<PickupItemSystem>();
    weaponAffectedByPickupSystem_     = coordinator->RegisterSystem<WeaponAffectedByPickupSystem>();
    playerHealthGUISystem_            = coordinator->RegisterSystem<PlayerHealthGUISystem>();
    smoothCameraFollowSystem_         = coordinator->RegisterSystem<SmoothCameraFollowSystem>();
    breakableBoxSystem_               = coordinator->RegisterSystem<BreakableBoxSystem>();
    spikeSystem_                      = coordinator->RegisterSystem<SpikeSystem>();

    // Set event callbacks for most of the systems.
    playerSystem_->eventCallback = 
    physicsSystem_->tilemapSystem->eventCallback =
    runningAnimationSystem_->eventCallback = 
    weaponSystem_->eventCallback =
    swingingAnimationSystem_->eventCallback = 
    weaponAffectedByAnimationSystem_->eventCallback =
    playerAffectedByAnimationSystem_->eventCallback =
        CC_BIND_EVENT_FUNC(MainLayer::OnEvent);

#pragma endregion
#pragma region SYSTEM_SIGNATURES
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

    Signature inventoryGUISystemSignature;
    inventoryGUISystemSignature.set(coordinator->GetComponentType<OwnedByComponent>());
    inventoryGUISystemSignature.set(coordinator->GetComponentType<ItemComponent>());
    coordinator->SetSystemSignature<InventoryGUISystem>(inventoryGUISystemSignature);

    Signature inventoryGUIHelperSystemSignature;
    inventoryGUIHelperSystemSignature.set(coordinator->GetComponentType<PlayerController2DComponent>());
    inventoryGUIHelperSystemSignature.set(coordinator->GetComponentType<InventoryComponent>());
    coordinator->SetSystemSignature<InventoryGUIHelperSystem>(inventoryGUIHelperSystemSignature);

    Signature pickupSystemSignature;
    pickupSystemSignature.set(coordinator->GetComponentType<ItemComponent>());
    pickupSystemSignature.set(coordinator->GetComponentType<PickupComponent>());
    coordinator->SetSystemSignature<PickupItemSystem>(pickupSystemSignature);

    Signature weaponAffectedByPickupSystemSignature;
    weaponAffectedByPickupSystemSignature.set(coordinator->GetComponentType<WeaponAffectedByPickupComponent>());
    weaponAffectedByPickupSystemSignature.set(coordinator->GetComponentType<WeaponComponent>());
    coordinator->SetSystemSignature<WeaponAffectedByPickupSystem>(weaponAffectedByPickupSystemSignature);

    Signature playerHealthGUISystemSignature;
    playerHealthGUISystemSignature.set(coordinator->GetComponentType<PlayerController2DComponent>());
    playerHealthGUISystemSignature.set(coordinator->GetComponentType<HealthComponent>());
    coordinator->SetSystemSignature<PlayerHealthGUISystem>(playerHealthGUISystemSignature);

    Signature smoothCameraFollowSystemSignature;
    smoothCameraFollowSystemSignature.set(coordinator->GetComponentType<PlayerController2DComponent>());
    smoothCameraFollowSystemSignature.set(coordinator->GetComponentType<TransformComponent>());
    coordinator->SetSystemSignature<SmoothCameraFollowSystem>(smoothCameraFollowSystemSignature);

    Signature breakableBoxSystemSignature;
    breakableBoxSystemSignature.set(coordinator->GetComponentType<BreakableComponent>());
    breakableBoxSystemSignature.set(coordinator->GetComponentType<HealthComponent>());
    breakableBoxSystemSignature.set(coordinator->GetComponentType<SpriteRendererComponent>());
    breakableBoxSystemSignature.set(coordinator->GetComponentType<PhysicsQuadtreeComponent>());
    coordinator->SetSystemSignature<BreakableBoxSystem>(breakableBoxSystemSignature);

    Signature spikeSystemSignature;
    spikeSystemSignature.set(coordinator->GetComponentType<SpikeComponent>());
    spikeSystemSignature.set(coordinator->GetComponentType<PhysicsQuadtreeComponent>());
    coordinator->SetSystemSignature<SpikeSystem>(spikeSystemSignature);

#pragma endregion
#pragma region TILEMAP_SETUP
    // TODO - Fix tilemap bleeding
    // TODO - Fix animation update
    constexpr glm::vec2 pixelAdventureTileSize = glm::vec2(16 ,16);
    auto grassTileTopLeft      = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 10), pixelAdventureTileSize);
    auto grassTileTopMiddle    = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 10), pixelAdventureTileSize);
    auto grassTileTopRight     = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 10), pixelAdventureTileSize);
    auto grassTileMiddleLeft   = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 9), pixelAdventureTileSize);
    auto grassTileMiddleMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 9), pixelAdventureTileSize);
    auto grassTileMiddleRight  = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 9), pixelAdventureTileSize);
    auto grassTileBottomLeft   = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 8), pixelAdventureTileSize);
    auto grassTileBottomMiddle = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 8),pixelAdventureTileSize);
    auto grassTileBottomRight  = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 8), pixelAdventureTileSize);
    auto platformStart         = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(17, 9), pixelAdventureTileSize);
    auto platformEnd           = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(19, 9), pixelAdventureTileSize);

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

#pragma endregion
#pragma region PLAYER_SETUP
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
        glm::vec2(0, -2),
        glm::vec2(7.0f, 14.f)
    ));

    constexpr glm::vec2 playerTextureSize = glm::vec2(32.f, 32.f);

    auto playerSpriteRendererComponent = SpriteRendererComponent();
    playerSpriteRendererComponent.Texture =
        (SubTexture2D::CreateFromCoords(playerSpritesheet_, glm::vec2(0, 8), playerTextureSize));
    runningAnimationSystem_->SetOriginalTexture(playerSpriteRendererComponent.Texture, playerEntity);
    swingingAnimationSystem_->SetOriginalTexture(playerSpriteRendererComponent.Texture, playerEntity);

    coordinator->AddComponent(playerEntity, playerSpriteRendererComponent);

    auto playerControllerComponent = PlayerController2DComponent();
    playerControllerComponent.ActiveMeleeWeaponIndices = { 3, 4 };
    coordinator->AddComponent(playerEntity, playerControllerComponent);

    coordinator->AddComponent(playerEntity, AffectedByAnimationComponent(Animation::AnimationType::Swinging));

#pragma region RUNNING_ANIMATION_SETUP

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

    runningAnimation.FramesBetweenTransition = 8;

    coordinator->AddComponent(playerEntity, runningAnimationComponent);

#pragma endregion

#pragma region SWINGING_ANIMATION_SETUP

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

    swingingAnimation.FramesBetweenTransition = 4;
    coordinator->AddComponent(playerEntity, swingingAnimationComponent);
#pragma endregion

#pragma region MELEE_WEAPON_SETUP
    // auto WeaponAffectedByAnimationSystem::DefaultMeleeAnimationBehaviour = 
    //     [](Entity weaponEntity, AnimationSpriteChangeEvent& ascEvent,
    //         const std::set<size_t>& activeIndices)
    //     {
    //         bool indexIsActive = activeIndices.find(ascEvent.GetSpriteIndex()) != activeIndices.end();
    //         coordinator->GetComponent<PhysicsQuadtreeComponent>(weaponEntity).Active = indexIsActive;
    //     };

    // auto WeaponAffectedByPickupSystem::DefaultMeleePickupBehaviour = [](Entity weaponEntity, PickupEvent& e)
    // {
    //     auto& weapon_component = coordinator->GetComponent<WeaponComponent>(weaponEntity);
    //     // NOTE - Every weapon should have a box collider, if it does not, how would it have been picked up anyway?
    //     auto& box_collider = coordinator->GetComponent<BoxCollider2DComponent>(weaponEntity);

    //     // Set the new extents to those of the correct use case when being held by the player.
    //     box_collider.Extents = weapon_component.EquippedExtents;
    // };

    auto woodenSwordWeaponEntity = coordinator->CreateEntity();

    WeaponComponent woodenSwordMeleeWeaponComponent;
    woodenSwordMeleeWeaponComponent.Behaviour = WeaponSystem::MeleeBehaviour;
    woodenSwordMeleeWeaponComponent.HandOffset = { 12, -2 };
    woodenSwordMeleeWeaponComponent.Damage = 5.0f;
    woodenSwordMeleeWeaponComponent.GroundExtents = { 10, 10 };
    woodenSwordMeleeWeaponComponent.EquippedExtents = { 4, 9 };
    coordinator->AddComponent(woodenSwordWeaponEntity, woodenSwordMeleeWeaponComponent);
    coordinator->AddComponent(woodenSwordWeaponEntity, WeaponAffectedByPickupComponent(WeaponAffectedByPickupSystem::DefaultMeleePickupBehaviour));
    coordinator->AddComponent(woodenSwordWeaponEntity, TransformComponent(glm::vec3(0.f, 0, 0),
        glm::vec3(0), glm::vec3(8, 18, 1)));
    coordinator->AddComponent(woodenSwordWeaponEntity, OwnedByComponent(playerEntity));
    coordinator->AddComponent(woodenSwordWeaponEntity, ItemComponent(
        SubTexture2D::CreateFromCoords(InventoryGUISystem::ItemSpritesheet, {0, 5}, {32, 32})
    ));

    WeaponAffectedByAnimationComponent meleeWeaponAffectedByAnimationComponent;

    // When the index is part of the weapons 'active index', enable the collider for melee weapons.
    meleeWeaponAffectedByAnimationComponent.AnimationBehaviour = WeaponAffectedByAnimationSystem::DefaultMeleeAnimationBehaviour;

    coordinator->AddComponent(woodenSwordWeaponEntity, meleeWeaponAffectedByAnimationComponent);

    RigidBody2DComponent meleeWeaponRigidbody;
    meleeWeaponRigidbody.BodyType = Physics::RigidBodyType::Static;

    coordinator->AddComponent(woodenSwordWeaponEntity, meleeWeaponRigidbody); 

    auto meleeBoxCollider = BoxCollider2DComponent();
    meleeBoxCollider.Extents = glm::vec2(4, 9);
    coordinator->AddComponent(woodenSwordWeaponEntity, meleeBoxCollider); 
    coordinator->AddComponent(woodenSwordWeaponEntity, PhysicsQuadtreeComponent()); 

    for (int i = 0; i < 6; ++i)
    {
        auto ironSwordWeaponEntity = coordinator->CreateEntity();

        WeaponComponent ironSwordMeleeWeaponComponent;
        ironSwordMeleeWeaponComponent.Behaviour = WeaponSystem::MeleeBehaviour;
        ironSwordMeleeWeaponComponent.HandOffset = { 12, -2 };
        ironSwordMeleeWeaponComponent.Damage = 12.0f;
        ironSwordMeleeWeaponComponent.GroundExtents = { 10, 10 };
        ironSwordMeleeWeaponComponent.EquippedExtents = { 4, 9 };

        coordinator->AddComponent(ironSwordWeaponEntity, ironSwordMeleeWeaponComponent);
        coordinator->AddComponent(ironSwordWeaponEntity, TransformComponent(glm::vec3(40 + i * 35, 20, -0.5f),
            glm::vec3(0), glm::vec3(32, 32, 1)));
        coordinator->AddComponent(ironSwordWeaponEntity, ItemComponent(
            SubTexture2D::CreateFromCoords(InventoryGUISystem::ItemSpritesheet, {1, 5}, {32, 32})));
        coordinator->AddComponent(ironSwordWeaponEntity, SpriteRendererComponent(
            SubTexture2D::CreateFromCoords(TransparentItemSpritesheet, {1, 21}, {32, 32})));
        coordinator->AddComponent(ironSwordWeaponEntity, WeaponAffectedByAnimationComponent(WeaponAffectedByAnimationSystem::DefaultMeleeAnimationBehaviour));
        coordinator->AddComponent(ironSwordWeaponEntity, RigidBody2DComponent()); 
        coordinator->AddComponent(ironSwordWeaponEntity, BoxCollider2DComponent({}, {10.f, 10.f})); 
        coordinator->AddComponent(ironSwordWeaponEntity, PhysicsQuadtreeComponent()); 
        coordinator->AddComponent(ironSwordWeaponEntity, PickupComponent()); 
        coordinator->AddComponent(ironSwordWeaponEntity, WeaponAffectedByPickupComponent(WeaponAffectedByPickupSystem::DefaultMeleePickupBehaviour));
    }
#pragma endregion

#pragma region INVENTORY_INIT
    auto inventoryComponent = InventoryComponent();
    inventoryComponent.Items.emplace_back(woodenSwordWeaponEntity);
    inventoryComponent.CurrentlyHolding = woodenSwordWeaponEntity;

    coordinator->AddComponent(playerEntity, inventoryComponent);
#pragma endregion

    coordinator->AddComponent(playerEntity, PhysicsQuadtreeComponent());
    coordinator->AddComponent(playerEntity, HealthComponent(100, 60,
    [](Entity e)
    {
        CC_TRACE("Player died.");
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour.a = 0.8f;
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour.a = 1.0f;
    }));
#pragma endregion
#pragma region MISC_ENTITIES
    auto testPhysicsEntity = coordinator->CreateEntity();
    coordinator->AddComponent(testPhysicsEntity, TransformComponent(glm::vec3(20, 0, 0),
       glm::vec3(0), glm::vec3(28, 24, 1)));
    coordinator->AddComponent(testPhysicsEntity, SpriteRendererComponent(
        SubTexture2D::CreateFromCoords(CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Items/Boxes/Box2/Idle.png"), glm::vec2(1, 1), glm::vec2(28, 24)))); 

    auto testPhysicsEntityCollider = BoxCollider2DComponent();
    testPhysicsEntityCollider.Extents = glm::vec2(10, 10);
    coordinator->AddComponent(testPhysicsEntity, testPhysicsEntityCollider); 
    coordinator->AddComponent(testPhysicsEntity, RigidBody2DComponent());
    coordinator->AddComponent(testPhysicsEntity, PhysicsQuadtreeComponent());
    coordinator->AddComponent(testPhysicsEntity, HealthComponent(20, 20, [](Entity e)
    {
        auto positionOfDestroyedEntity = coordinator->GetComponent<TransformComponent>(e).Position;
        coordinator->DestroyEntity(e);
        auto ironSwordWeaponEntity = coordinator->CreateEntity();

        WeaponComponent ironSwordMeleeWeaponComponent;
        ironSwordMeleeWeaponComponent.Behaviour = WeaponSystem::MeleeBehaviour;
        ironSwordMeleeWeaponComponent.HandOffset = { 12, -2 };
        ironSwordMeleeWeaponComponent.Damage = 12.0f;
        ironSwordMeleeWeaponComponent.GroundExtents = { 10, 10 };
        ironSwordMeleeWeaponComponent.EquippedExtents = { 4, 9 };

        CC_TRACE("Iron sword entity no: ", ironSwordWeaponEntity);
        constexpr glm::vec2 offset = {0, 6.0f};
        coordinator->AddComponent(ironSwordWeaponEntity, ironSwordMeleeWeaponComponent);
        coordinator->AddComponent(ironSwordWeaponEntity, TransformComponent(glm::vec3(glm::vec2(positionOfDestroyedEntity) + offset, -0.5f),
            glm::vec3(0), glm::vec3(32, 32, 1)));
        coordinator->AddComponent(ironSwordWeaponEntity, ItemComponent(
            SubTexture2D::CreateFromCoords(InventoryGUISystem::ItemSpritesheet, {1, 5}, {32, 32})));
        coordinator->AddComponent(ironSwordWeaponEntity, SpriteRendererComponent(
            SubTexture2D::CreateFromCoords(TransparentItemSpritesheet, {1, 21}, {32, 32})));
        coordinator->AddComponent(ironSwordWeaponEntity, WeaponAffectedByAnimationComponent(WeaponAffectedByAnimationSystem::DefaultMeleeAnimationBehaviour));

        auto rigidbody = RigidBody2DComponent();
        rigidbody.LinearVelocity.y = 40.0f;
        coordinator->AddComponent(ironSwordWeaponEntity, rigidbody); 
        coordinator->AddComponent(ironSwordWeaponEntity, BoxCollider2DComponent({}, {10.f, 10.f})); 
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        physics_quadtree_component.Active = true;
        coordinator->AddComponent(ironSwordWeaponEntity, physics_quadtree_component); 
        coordinator->AddComponent(ironSwordWeaponEntity, PickupComponent()); 
        coordinator->AddComponent(ironSwordWeaponEntity, WeaponAffectedByPickupComponent(WeaponAffectedByPickupSystem::DefaultMeleePickupBehaviour));
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 0.9f, 0.5f, 0.5f, 1.0f };
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    }));
    coordinator->AddComponent(testPhysicsEntity, BreakableComponent(8));

#pragma region SPIKE_SETUP
    for (int i = 0; i < 4; ++i)
    {
        auto spikeEntity = coordinator->CreateEntity();
        coordinator->AddComponent(spikeEntity, SpikeComponent(10.f));
        auto spikeRigidbody = RigidBody2DComponent();
        spikeRigidbody.BodyType = Physics::RigidBodyType::Static;
        coordinator->AddComponent(spikeEntity, TransformComponent(glm::vec3(-176 + i * 16, 42, -0.5f),
            glm::vec3(0), glm::vec3(16, 16, 1)));
        coordinator->AddComponent(spikeEntity, SpriteRendererComponent(SubTexture2D::CreateFromCoords(spikeTexture_, {1.00f, 1.00f}, {16.00f, 16.00f})));
        coordinator->AddComponent(spikeEntity, spikeRigidbody); 
        coordinator->AddComponent(spikeEntity, BoxCollider2DComponent({0, -4.f}, {16.f, 6.f})); 
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        physics_quadtree_component.Active = true;
        coordinator->AddComponent(spikeEntity, physics_quadtree_component); 

    }
#pragma endregion
#pragma endregion
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
    // cameraController_.OnUpdate(ts);
    weaponSystem_->Update(ts);
    damageableSystem_->Update(ts);
    physicsSystem_->Update(ts);
    playerSystem_->Update(ts);
    cameraController_.GetCamera().SetPosition(
        smoothCameraFollowSystem_->GetCalculatedPosition(ts));

    constexpr glm::vec4 background_colour = glm::vec4(135.f, 206.f, 250.f, 1.0f);
    RenderCommand::SetClearColour(Utility::Colour32BitConvert(background_colour));
    RenderCommand::Clear();

    Renderer2D::BeginScene(cameraController_.GetCamera());

    constexpr float tiling_factor = 128.0f;
    Renderer2D::DrawQuad(glm::vec3(), glm::vec2(2560, 2560), brownBackground_, tiling_factor);

    runningAnimationSystem_->Update(ts);
    swingingAnimationSystem_->Update(ts);

    tilemapRenderSystem_->Update(ts);
    spriteRenderSystem_->Update(ts);

    Renderer2D::EndScene();
}


auto MainLayer::OnEvent(Event &e) -> void 
{
#define OEB(x, y) std::bind(&x::OnEvent, y, std::placeholders::_1)
    static const std::array<std::function<void(Event&)>, 13> on_events {
        OEB(PhysicsSystem, physicsSystem_),
        OEB(WeaponSystem, weaponSystem_),

        OEB(OrthographicCameraController, &cameraController_),

        OEB(BreakableBoxSystem, breakableBoxSystem_),
        OEB(DamageableSystem, damageableSystem_),

        OEB(PlayerSystem, playerSystem_),
        OEB(PickupItemSystem, pickupSystem_),
        OEB(SpikeSystem, spikeSystem_),
        OEB(WeaponAffectedByPickupSystem, weaponAffectedByPickupSystem_),

        OEB(RunningAnimationSystem, runningAnimationSystem_),
        OEB(SwingingAnimationSystem, swingingAnimationSystem_),
        OEB(PlayerAffectedByAnimationSystem, playerAffectedByAnimationSystem_),
        OEB(WeaponAffectedByAnimationSystem, weaponAffectedByAnimationSystem_)
    };
#undef OEB 

    for (auto& func : on_events)
    {
        if (e.Handled)
            return;
        func(e);
    }
}

auto MainLayer::OnImGuiRender() -> void 
{
    inventoryGUISystem_->OnImGuiRender();
    playerHealthGUISystem_->OnImGuiRender();
}
