#include "MainLayer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"
#include "Utils/SoundController.h"
#include "Utils/MusicPlayer.h"
#include <string>

#include "Audio/Transition.h"

#include "Core/KeyCodes.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

#include "GameManager.h"

#include <type_traits>

#include "Physics/PhysicsComponent.h"

#include "Utils/Input.h"
#include "Utils/Util.h"

#include <examples/imgui_impl_opengl3.h>

#include "imgui.h"

static Coordinator* coordinator = Coordinator::Instance();
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
    buttons_ = CreateRef<Texture2D>("Assets/Images/buttons_4x.png");
    batSpritesheet_ = CreateRef<Texture2D>("Assets/Spritesheets/Bat_Sprite_Sheet.png");
    nightborneSpritesheet_ = CreateRef<Texture2D>("Assets/Spritesheets/NightBorne.png");

    playButton_ = SubTexture2D::CreateFromCoords(buttons_, { 13, 1 }, {32, 32});
    pauseButton_ = SubTexture2D::CreateFromCoords(buttons_, { 12, 1 }, {32, 32});

#pragma region SOUND 
    auto soundController = SoundController::Instance();
    for (int i = 0; i < 9; ++i)
        soundController->MasterVolumeDecrease();
    

    // soundController->LoadSound("Assets/Sounds/01.-The-Tale-of-a-Cruel-World.ogg", 1, true);
    // MusicPlayer::Instance()->AddMusic("Assets/Sounds/calamitybgm.mp3", 1, true);
    // MusicPlayer::Instance()->AddMusic("Assets/Sounds/01.-The-Tale-of-a-Cruel-World.ogg", 1, true);

#pragma endregion 

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
    coordinator->RegisterComponent<FlyingAnimationComponent>();

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
    coordinator->RegisterComponent<HealthPotionComponent>();
    coordinator->RegisterComponent<PortalComponent>();
    coordinator->RegisterComponent<ReferenceComponent>();
    coordinator->RegisterComponent<ProjectileComponent>();
    coordinator->RegisterComponent<EntityQueueComponent>();
    coordinator->RegisterComponent<BatComponent>();
    coordinator->RegisterComponent<NightborneComponent>();
    coordinator->RegisterComponent<BringerComponent>();

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
    healingPotionSystem_              = coordinator->RegisterSystem<HealingPotionSystem>();
    portalSystem_                     = coordinator->RegisterSystem<PortalSystem>();
    projectileSystem_                 = coordinator->RegisterSystem<ProjectileSystem>();
    batFlyingAnimationSystem_         = coordinator->RegisterSystem<BatFlyingAnimationSystem>();
    batSystem_                        = coordinator->RegisterSystem<BatSystem>();
    nightborneSystem_                 = coordinator->RegisterSystem<NightborneSystem>(); 
    bringerSystem_                    = coordinator->RegisterSystem<BringerSystem>(); 

    // Set event callbacks for most of the systems.
    playerSystem_->eventCallback = 
    physicsSystem_->eventCallback = 
    physicsSystem_->tilemapSystem->eventCallback =
    runningAnimationSystem_->eventCallback = 
    weaponSystem_->eventCallback =
    swingingAnimationSystem_->eventCallback = 
    weaponAffectedByAnimationSystem_->eventCallback =
    playerAffectedByAnimationSystem_->eventCallback =
    healingPotionSystem_->eventCallback =
    projectileSystem_->eventCallback =
    batFlyingAnimationSystem_->eventCallback =
    batSystem_->eventCallback =
    nightborneSystem_->eventCallback = 
    bringerSystem_->eventCallback = 
        CC_BIND_EVENT_FUNC(MainLayer::OnEvent);

    physicsSystem_->helperSystem = inventoryGUISystem_->helperSystem;
    batSystem_->playerSystem = playerSystem_;
    nightborneSystem_->playerSystem = playerSystem_;
    bringerSystem_->playerSystem = playerSystem_;

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

    Signature batFlyingAnimationSystemSignature;
    batFlyingAnimationSystemSignature.set(coordinator->GetComponentType<FlyingAnimationComponent>());
    batFlyingAnimationSystemSignature.set(coordinator->GetComponentType<SpriteRendererComponent>());
    coordinator->SetSystemSignature<BatFlyingAnimationSystem>(batFlyingAnimationSystemSignature);

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

    Signature healingPotionSystemSignature;
    healingPotionSystemSignature.set(coordinator->GetComponentType<HealthPotionComponent>());
    healingPotionSystemSignature.set(coordinator->GetComponentType<OwnedByComponent>());
    coordinator->SetSystemSignature<HealingPotionSystem>(healingPotionSystemSignature);

    Signature portalSystemSignature;
    portalSystemSignature.set(coordinator->GetComponentType<PortalComponent>());
    coordinator->SetSystemSignature<PortalSystem>(portalSystemSignature);

    Signature projectileSystemSignature;
    projectileSystemSignature.set(coordinator->GetComponentType<ProjectileComponent>());
    coordinator->SetSystemSignature<ProjectileSystem>(projectileSystemSignature);

    Signature batSystemSignature;
    batSystemSignature.set(coordinator->GetComponentType<BatComponent>());
    coordinator->SetSystemSignature<BatSystem>(batSystemSignature);

    Signature nightborneSystemSignature;
    nightborneSystemSignature.set(coordinator->GetComponentType<NightborneComponent>());
    coordinator->SetSystemSignature<NightborneSystem>(nightborneSystemSignature);

    Signature bringerSystemSignature;
    bringerSystemSignature.set(coordinator->GetComponentType<BringerComponent>());
    coordinator->SetSystemSignature<BringerSystem>(bringerSystemSignature);

#pragma endregion

    // soundController->PlaySoundByID(1);
    // MusicPlayer::Instance()->PlayMusicByID(1);
}

auto MainLayer::OnAttach() -> void 
{
    timer_ = 0;
    musicTimer_ = 0;
#pragma region TILEMAP_SETUP
    // TODO - Fix tilemap bleeding
    // TODO - Fix animation update
    constexpr glm::vec2 pixelAdventureTileSize = glm::vec2(16 ,16);
    auto grassTileTopLeft       = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 10), pixelAdventureTileSize);
    auto grassTileTopMiddle     = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 10), pixelAdventureTileSize);
    auto grassTileTopRight      = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 10), pixelAdventureTileSize);
    auto grassTileMiddleLeft    = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 9), pixelAdventureTileSize);
    auto grassTileMiddleMiddle  = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 9), pixelAdventureTileSize);
    auto grassTileMiddleRight   = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 9), pixelAdventureTileSize);
    auto grassTileBottomLeft    = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(6, 8), pixelAdventureTileSize);
    auto grassTileBottomMiddle  = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(7, 8),pixelAdventureTileSize);
    auto grassTileBottomRight   = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(8, 8), pixelAdventureTileSize);
    auto platformStart          = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(17, 9), pixelAdventureTileSize);
    auto platformEnd            = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(19, 9), pixelAdventureTileSize);
    auto goldBlockStart         = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(17, 2), pixelAdventureTileSize);
    auto goldBlockMiddle        = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(18, 2), pixelAdventureTileSize);
    auto goldBlockEnd           = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(19, 2), pixelAdventureTileSize);
    // auto silverBlockBottomRight = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(14, 4), pixelAdventureTileSize);
    // auto silverBlockBottomLeft  = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(13, 4), pixelAdventureTileSize);
    // auto silverBlockTopRight    = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(14, 5), pixelAdventureTileSize);
    // auto silverBlockTopLeft     = SubTexture2D::CreateFromCoords(terrainSpritesheet_, glm::vec2(13, 5), pixelAdventureTileSize);

    platformStart->InsetInwardsY(0.001f);

    platformEnd->InsetInwardsY(0.001f);

    auto tilemapEntity = coordinator->CreateEntity();

    TilemapComponent tilemapComponent("Assets/Maps/TestMap.csv", "Assets/Maps/TestMapTypes.csv");
    tilemapComponent.SubTextureMap[1 ] = grassTileTopLeft;
    tilemapComponent.SubTextureMap[2 ] = grassTileTopMiddle;
    tilemapComponent.SubTextureMap[3 ] = grassTileTopRight;
    tilemapComponent.SubTextureMap[4 ] = grassTileMiddleLeft;
    tilemapComponent.SubTextureMap[5 ] = grassTileMiddleMiddle;
    tilemapComponent.SubTextureMap[6 ] = grassTileMiddleRight;
    tilemapComponent.SubTextureMap[7 ] = grassTileBottomLeft;
    tilemapComponent.SubTextureMap[8 ] = grassTileBottomMiddle;
    tilemapComponent.SubTextureMap[9 ] = grassTileBottomRight;
    tilemapComponent.SubTextureMap[10] = platformStart;
    tilemapComponent.SubTextureMap[11] = platformEnd;
    tilemapComponent.SubTextureMap[12] = goldBlockStart;
    tilemapComponent.SubTextureMap[13] = goldBlockMiddle;
    tilemapComponent.SubTextureMap[14] = goldBlockEnd;
    // tilemapComponent.SubTextureMap[15] = silverBlockBottomLeft;
    // tilemapComponent.SubTextureMap[16] = silverBlockTopLeft;
    // tilemapComponent.SubTextureMap[17] = silverBlockTopRight; 
    // tilemapComponent.SubTextureMap[18] = silverBlockBottomRight;

    coordinator->AddComponent(tilemapEntity, TransformComponent {
        glm::vec3(TilemapData::TILEMAP_MAX_X_LENGTH * -0.5f * tilemapComponent.TileSize.x, -70, 0),
        // glm::vec3(0, 0, 0),
    });

    coordinator->AddComponent(tilemapEntity, std::move(tilemapComponent));

    // auto tilemapEntity2 = coordinator->CreateEntity();

    // TilemapComponent tilemapComponent2("Assets/Maps/TestMap.csv", "Assets/Maps/TestMapTypes.csv");
    // tilemapComponent2.SubTextureMap[1] = grassTileTopLeft;
    // tilemapComponent2.SubTextureMap[2] = grassTileTopMiddle;
    // tilemapComponent2.SubTextureMap[3] = grassTileTopRight;
    // tilemapComponent2.SubTextureMap[4] = grassTileMiddleLeft;
    // tilemapComponent2.SubTextureMap[5] = grassTileMiddleMiddle;
    // tilemapComponent2.SubTextureMap[6] = grassTileMiddleRight;
    // tilemapComponent2.SubTextureMap[7] = grassTileBottomLeft;
    // tilemapComponent2.SubTextureMap[8] = grassTileBottomMiddle;
    // tilemapComponent2.SubTextureMap[9] = grassTileBottomRight;
    // tilemapComponent2.SubTextureMap[10] = platformStart;
    // tilemapComponent2.SubTextureMap[11] = platformEnd;

    // coordinator->AddComponent(tilemapEntity2, TransformComponent {
    //     glm::vec3(TilemapData::TILEMAP_MAX_X_LENGTH * -1.5f * tilemapComponent.TileSize.x, -70, 0),
    // });

    // coordinator->AddComponent(tilemapEntity2, tilemapComponent2);

#pragma endregion
#pragma region PLAYER_SETUP
    auto playerEntity = coordinator->CreateEntity();

    coordinator->AddComponent(playerEntity, TransformComponent{
        // glm::vec3(-100, 500, 0), 
        glm::vec3(48, -3, 0), 
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

    constexpr glm::vec2 playerTextureSize = glm::vec2(32.0f, 32.0f);

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
        auto texture = 
            SubTexture2D::CreateFromCoords(playerSpritesheet_, glm::vec2(i , 6), playerTextureSize);

        // texture inset 
        texture->Inset(0.001f);

        runningAnimation.SpriteTextures.emplace_back(texture);
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
        auto texture = 
            SubTexture2D::CreateFromCoords(playerSpritesheet_, glm::vec2(i , 0), playerTextureSize);

        texture->Inset(0.001f);

        swingingAnimation.SpriteTextures.emplace_back(texture);
    }

    for (size_t i = 0; i < swingingAnimation.SpriteTextures.size(); ++i)
    {
        swingingAnimation.AnimationIndices.emplace_back(i);
    }

    swingingAnimation.FramesBetweenTransition = 4;
    coordinator->AddComponent(playerEntity, swingingAnimationComponent);
#pragma endregion

#pragma region MAGE_WEAPON_SETUP
    



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
    woodenSwordMeleeWeaponComponent.ActiveBehaviour = WeaponSystem::MeleeActiveBehaviour;
    woodenSwordMeleeWeaponComponent.HandOffset = { 12, -2 };
    woodenSwordMeleeWeaponComponent.Damage = 5.0f;
    woodenSwordMeleeWeaponComponent.GroundExtents = { 10, 10 };
    woodenSwordMeleeWeaponComponent.EquippedExtents = { 4, 9 };
    woodenSwordMeleeWeaponComponent.DamageCondition = WeaponSystem::MeleeDamageCondition;
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

    for (int i = 0; i < 1; ++i)
    {
        auto ironSwordWeaponEntity = coordinator->CreateEntity();

        WeaponComponent ironSwordMeleeWeaponComponent;
        ironSwordMeleeWeaponComponent.Behaviour = WeaponSystem::MeleeBehaviour;
        ironSwordMeleeWeaponComponent.ActiveBehaviour = WeaponSystem::MeleeActiveBehaviour;
        ironSwordMeleeWeaponComponent.DamageCondition = WeaponSystem::MeleeDamageCondition;
        ironSwordMeleeWeaponComponent.HandOffset = { 12, -2 };
        ironSwordMeleeWeaponComponent.Damage = 20.0f;
        ironSwordMeleeWeaponComponent.GroundExtents = { 10, 10 };
        ironSwordMeleeWeaponComponent.EquippedExtents = { 4, 9 };

        coordinator->AddComponent(ironSwordWeaponEntity, ironSwordMeleeWeaponComponent);
        coordinator->AddComponent(ironSwordWeaponEntity, TransformComponent(glm::vec3(-909, 30, -0.5f),
            glm::vec3(0), glm::vec3(32, 32, 1)));
        coordinator->AddComponent(ironSwordWeaponEntity, ItemComponent(
            SubTexture2D::CreateFromCoords(InventoryGUISystem::ItemSpritesheet, {2, 5}, {32, 32})));
        coordinator->AddComponent(ironSwordWeaponEntity, SpriteRendererComponent(
            SubTexture2D::CreateFromCoords(TransparentItemSpritesheet, {2, 21}, {32, 32})));
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
    // inventoryComponent.Items.emplace_back(starterWandEntity);
    inventoryComponent.CurrentlyHolding = woodenSwordWeaponEntity;

    coordinator->AddComponent(playerEntity, inventoryComponent);
#pragma endregion

    coordinator->AddComponent(playerEntity, PhysicsQuadtreeComponent());
    coordinator->AddComponent(playerEntity, HealthComponent(100, 60,
    [](Entity e)
    {
        auto& health = coordinator->GetComponent<HealthComponent>(e);
        health.Health = health.MaxHealth;
        auto& transform = coordinator->GetComponent<TransformComponent>(e);
        transform.Position = glm::vec3(48, -3, 0);
        SoundController::Instance()->PlaySoundByID(11, false);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour.a = 0.5f;
        SoundController::Instance()->PlaySoundByID(11, true);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour.a = 1.0f;
    }));
#pragma endregion
#pragma region MISC_ENTITIES
    // TODO - Box blocking.
    auto blockingEntity = coordinator->CreateEntity(); 
    coordinator->AddComponent(blockingEntity, TransformComponent(glm::vec3(-24, 130, 0),
       glm::vec3(0), glm::vec3(32, 32, 1)));
    coordinator->AddComponent(blockingEntity, SpriteRendererComponent(
        SubTexture2D::CreateFromCoords(CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Terrain/Terrain (16x16).png"), glm::vec2(6.5f, 2), glm::vec2(32, 32)))); 
    coordinator->AddComponent(blockingEntity, BoxCollider2DComponent({0, 0}, {16, 16})); 
    coordinator->AddComponent(blockingEntity, RigidBody2DComponent(Physics::RigidBodyType::Static));
    coordinator->AddComponent(blockingEntity, PhysicsQuadtreeComponent());
    coordinator->AddComponent(blockingEntity, ReferenceComponent(tilemapEntity));
    coordinator->AddComponent(blockingEntity, HealthComponent(100, 20, [](Entity e)
    {
        auto positionOfDestroyedEntity = coordinator->GetComponent<TransformComponent>(e).Position;
        auto& tilemap = coordinator->GetComponent<TilemapComponent>(coordinator->GetComponent<ReferenceComponent>(e).RefEntity);

        tilemap.MapData[12][62].Type = Tile::TileType::Empty;
        tilemap.MapData[12][63].Type = Tile::TileType::Empty;
        tilemap.MapData[13][62].Type = Tile::TileType::Empty;
        tilemap.MapData[13][63].Type = Tile::TileType::Empty;

        coordinator->DestroyEntity(e);
        SoundController::Instance()->PlaySoundByID(10, true);
         
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 0.9f, 0.5f, 0.5f, 1.0f };
        SoundController::Instance()->PlaySoundByID(10, true);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    }));
    coordinator->AddComponent(blockingEntity, BreakableComponent(20.0f));

    auto blockingEntity2 = coordinator->CreateEntity(); 
    coordinator->AddComponent(blockingEntity2, TransformComponent(glm::vec3(-656, 633, 0),
       glm::vec3(0), glm::vec3(48, 48, 1)));
    coordinator->AddComponent(blockingEntity2, SpriteRendererComponent(
        SubTexture2D::CreateFromCoords(CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Terrain/Terrain (16x16).png"), glm::vec2(6.5f, 2), glm::vec2(32, 32)))); 
    coordinator->AddComponent(blockingEntity2, BoxCollider2DComponent({0, 0}, {28, 28})); 
    coordinator->AddComponent(blockingEntity2, RigidBody2DComponent(Physics::RigidBodyType::Static));
    coordinator->AddComponent(blockingEntity2, PhysicsQuadtreeComponent());
    coordinator->AddComponent(blockingEntity2, ReferenceComponent(tilemapEntity));
    coordinator->AddComponent(blockingEntity2, HealthComponent(10, 20, [](Entity e)
    {
        auto& tilemap = coordinator->GetComponent<TilemapComponent>(coordinator->GetComponent<ReferenceComponent>(e).RefEntity);

        tilemap.MapData[43][22].Type = Tile::TileType::Empty;
        tilemap.MapData[44][22].Type = Tile::TileType::Empty;
        tilemap.MapData[45][22].Type = Tile::TileType::Empty;
        tilemap.MapData[43][23].Type = Tile::TileType::Empty;
        tilemap.MapData[44][23].Type = Tile::TileType::Empty;
        tilemap.MapData[45][23].Type = Tile::TileType::Empty;
        tilemap.MapData[43][24].Type = Tile::TileType::Empty;
        tilemap.MapData[44][24].Type = Tile::TileType::Empty;
        tilemap.MapData[45][24].Type = Tile::TileType::Empty;

        coordinator->DestroyEntity(e);
        SoundController::Instance()->PlaySoundByID(10, true);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 0.9f, 0.5f, 0.5f, 1.0f };
        SoundController::Instance()->PlaySoundByID(10, true);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    }));
    coordinator->AddComponent(blockingEntity2, BreakableComponent(3.0f));

    auto blockingEntity3 = coordinator->CreateEntity(); 
    coordinator->AddComponent(blockingEntity3, TransformComponent(glm::vec3(-464, 697, 0),
       glm::vec3(0), glm::vec3(48, 48, 1)));
    coordinator->AddComponent(blockingEntity3, SpriteRendererComponent(
        SubTexture2D::CreateFromCoords(CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Terrain/Terrain (16x16).png"), glm::vec2(6.5f, 2), glm::vec2(32, 32)))); 
    coordinator->AddComponent(blockingEntity3, BoxCollider2DComponent({0, 0}, {28, 28})); 
    coordinator->AddComponent(blockingEntity3, RigidBody2DComponent(Physics::RigidBodyType::Static));
    coordinator->AddComponent(blockingEntity3, PhysicsQuadtreeComponent());
    coordinator->AddComponent(blockingEntity3, ReferenceComponent(tilemapEntity));
    coordinator->AddComponent(blockingEntity3, HealthComponent(10, 20, [](Entity e)
    {
        auto& tilemap = coordinator->GetComponent<TilemapComponent>(coordinator->GetComponent<ReferenceComponent>(e).RefEntity);

        tilemap.MapData[47][34].Type = Tile::TileType::Empty;
        tilemap.MapData[48][34].Type = Tile::TileType::Empty;
        tilemap.MapData[49][34].Type = Tile::TileType::Empty;
        tilemap.MapData[47][35].Type = Tile::TileType::Empty;
        tilemap.MapData[48][35].Type = Tile::TileType::Empty;
        tilemap.MapData[49][35].Type = Tile::TileType::Empty;
        tilemap.MapData[47][36].Type = Tile::TileType::Empty;
        tilemap.MapData[48][36].Type = Tile::TileType::Empty;
        tilemap.MapData[49][36].Type = Tile::TileType::Empty;

        coordinator->DestroyEntity(e);
        SoundController::Instance()->PlaySoundByID(10, true);
         
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 0.9f, 0.5f, 0.5f, 1.0f };
        SoundController::Instance()->PlaySoundByID(10, true);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    }));
    coordinator->AddComponent(blockingEntity3, BreakableComponent(3.0f));

    auto testPhysicsEntity = coordinator->CreateEntity();
    coordinator->AddComponent(testPhysicsEntity, TransformComponent(glm::vec3(-10, 0, 0),
       glm::vec3(0), glm::vec3(28, 24, 1)));
    coordinator->AddComponent(testPhysicsEntity, SpriteRendererComponent(
        SubTexture2D::CreateFromCoords(CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Items/Boxes/Box2/Idle.png"), glm::vec2(1, 1), glm::vec2(28, 24)))); 

    auto testPhysicsEntityCollider = BoxCollider2DComponent();
    testPhysicsEntityCollider.Extents = glm::vec2(10, 10);
    coordinator->AddComponent(testPhysicsEntity, testPhysicsEntityCollider); 
    coordinator->AddComponent(testPhysicsEntity, RigidBody2DComponent(Physics::RigidBodyType::Dynamic));
    coordinator->AddComponent(testPhysicsEntity, PhysicsQuadtreeComponent());
    coordinator->AddComponent(testPhysicsEntity, HealthComponent(20, 20, [](Entity e)
    {
        auto positionOfDestroyedEntity = coordinator->GetComponent<TransformComponent>(e).Position;
        coordinator->DestroyEntity(e);
        auto starterWandEntity = coordinator->CreateEntity();
        auto wandSparkEntity = coordinator->CreateEntity();
        coordinator->AddComponent(wandSparkEntity, TransformComponent({}, {}, {6, 2, 1}));
        coordinator->AddComponent(wandSparkEntity, SpriteRendererComponent(Utility::Colour32BitConvert({255.f, 252.f, 127.f, 0.f})));
        coordinator->AddComponent(wandSparkEntity, RigidBody2DComponent(Physics::RigidBodyType::Kinematic));
        coordinator->AddComponent(wandSparkEntity, PhysicsQuadtreeComponent(false));
        coordinator->AddComponent(wandSparkEntity, BoxCollider2DComponent({}, {3, 1}));

        // To return
        coordinator->AddComponent(wandSparkEntity, ProjectileComponent(starterWandEntity, 150.0f));

        auto wandSparkEntity2 = coordinator->CreateEntity();
        coordinator->AddComponent(wandSparkEntity2, TransformComponent({}, {}, {6, 2, 1}));
        coordinator->AddComponent(wandSparkEntity2, SpriteRendererComponent(Utility::Colour32BitConvert({255.f, 252.f, 127.f, 0.f})));
        coordinator->AddComponent(wandSparkEntity2, RigidBody2DComponent(Physics::RigidBodyType::Kinematic));
        coordinator->AddComponent(wandSparkEntity2, PhysicsQuadtreeComponent(false));
        coordinator->AddComponent(wandSparkEntity2, BoxCollider2DComponent({}, {3, 1}));
        coordinator->AddComponent(wandSparkEntity2, ProjectileComponent(starterWandEntity, 150.0f));

        WeaponComponent starterWandWeaponComponent;
        starterWandWeaponComponent.Behaviour = WeaponSystem::MageBehaviour;
        starterWandWeaponComponent.ActiveBehaviour = WeaponSystem::MageActiveBehaviour;
        starterWandWeaponComponent.HandOffset = { 12, -2 };
        starterWandWeaponComponent.Damage = 5.0f;

        coordinator->AddComponent(starterWandEntity, EntityQueueComponent({ wandSparkEntity, wandSparkEntity2 }));
        coordinator->AddComponent(starterWandEntity, PhysicsQuadtreeComponent()); 
        coordinator->AddComponent(starterWandEntity, BoxCollider2DComponent({}, {10, 10})); 
        coordinator->AddComponent(starterWandEntity, WeaponComponent(starterWandWeaponComponent));
        coordinator->AddComponent(starterWandEntity, TransformComponent(glm::vec3(0.f, 0, 0),
            glm::vec3(0), glm::vec3(32, 32, 1)));
        // coordinator->AddComponent(starterWandEntity, OwnedByComponent(playerEntity));
        coordinator->AddComponent(starterWandEntity, ItemComponent(
            SubTexture2D::CreateFromCoords(InventoryGUISystem::ItemSpritesheet, {8, 6}, {32, 32})
        ));
        coordinator->AddComponent(starterWandEntity, SpriteRendererComponent(
            SubTexture2D::CreateFromCoords(TransparentItemSpritesheet, {8, 20}, {32, 32})));
        auto rigidbody = RigidBody2DComponent();
        rigidbody.LinearVelocity.y = 40.0f;
        coordinator->AddComponent(starterWandEntity, rigidbody); 
        coordinator->AddComponent(starterWandEntity, PickupComponent()); 
        coordinator->AddComponent(starterWandEntity, WeaponAffectedByPickupComponent(WeaponAffectedByPickupSystem::DefaultMeleePickupBehaviour));

        SoundController::Instance()->GetSound(8)->SetPosition(positionOfDestroyedEntity.x, positionOfDestroyedEntity.y, 0);
        SoundController::Instance()->PlaySoundByID(8, true);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 0.9f, 0.5f, 0.5f, 1.0f };
        auto position = coordinator->GetComponent<TransformComponent>(e).Position;
        SoundController::Instance()->GetSound(7)->SetPosition(position.x, position.y, 0);
        SoundController::Instance()->PlaySoundByID(7, true);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    }));
    coordinator->AddComponent(testPhysicsEntity, BreakableComponent(5));

    auto portalEntity = coordinator->CreateEntity();
    coordinator->AddComponent(portalEntity, TransformComponent(glm::vec3(29.5f, 130.f, 0), glm::vec3(0), glm::vec3(30, 30, 1)));
    coordinator->AddComponent(portalEntity, SpriteRendererComponent(
        CreateRef<SubTexture2D>(CreateRef<Texture2D>("Assets/Images/portal.png"), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f)))); 
    coordinator->AddComponent(portalEntity, PortalComponent([](Entity e, PlayerEnterEvent& event)
    {
        auto& player_transform = coordinator->GetComponent<TransformComponent>(event.GetPlayerEntity());
        player_transform.Position.x = -960;
        player_transform.Position.y = 460;
    }));
    coordinator->AddComponent(portalEntity, BoxCollider2DComponent({}, {8, 8}));
    coordinator->AddComponent(portalEntity, RigidBody2DComponent(Physics::RigidBodyType::Static));
    coordinator->AddComponent(portalEntity, PhysicsQuadtreeComponent());

    auto portalEntity2 = coordinator->CreateEntity();
    coordinator->AddComponent(portalEntity2, TransformComponent(glm::vec3(-980, 460.f, 0), glm::vec3(0), glm::vec3(30, 30, 1)));
    coordinator->AddComponent(portalEntity2, SpriteRendererComponent(
        CreateRef<SubTexture2D>(CreateRef<Texture2D>("Assets/Images/portal.png"), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f)))); 
    coordinator->AddComponent(portalEntity2, PortalComponent([](Entity e, PlayerEnterEvent& event)
    {
        auto& player_transform = coordinator->GetComponent<TransformComponent>(event.GetPlayerEntity());
        player_transform.Position.x = 7;
        player_transform.Position.y = 130;
    }));
    coordinator->AddComponent(portalEntity2, BoxCollider2DComponent({}, {8, 8}));
    coordinator->AddComponent(portalEntity2, RigidBody2DComponent(Physics::RigidBodyType::Static));
    coordinator->AddComponent(portalEntity2, PhysicsQuadtreeComponent());

    
    auto healthPotionBoxCave = coordinator->CreateEntity();
    coordinator->AddComponent(healthPotionBoxCave, TransformComponent(glm::vec3(-520, 202, 0),
       glm::vec3(0), glm::vec3(28, 24, 1)));
    coordinator->AddComponent(healthPotionBoxCave, SpriteRendererComponent(
        SubTexture2D::CreateFromCoords(CreateRef<Texture2D>("Assets/Spritesheets/PixelAdventure1/Items/Boxes/Box2/Idle.png"), glm::vec2(1, 1), glm::vec2(28, 24)))); 

    auto hpbEntityCollider = BoxCollider2DComponent();
    hpbEntityCollider.Extents = glm::vec2(10, 10);
    coordinator->AddComponent(healthPotionBoxCave, hpbEntityCollider); 
    coordinator->AddComponent(healthPotionBoxCave, RigidBody2DComponent());
    coordinator->AddComponent(healthPotionBoxCave, PhysicsQuadtreeComponent());
    coordinator->AddComponent(healthPotionBoxCave, HealthComponent(20, 20, [](Entity e)
    {
        auto positionOfDestroyedEntity = coordinator->GetComponent<TransformComponent>(e).Position;
        coordinator->DestroyEntity(e);

        auto healthPot = coordinator->CreateEntity();

        constexpr glm::vec2 offset = {0, 6.0f};
        coordinator->AddComponent(healthPot, TransformComponent(glm::vec3(glm::vec2(positionOfDestroyedEntity) + offset, -0.5f),
            glm::vec3(0), glm::vec3(32, 32, 1)));
        coordinator->AddComponent(healthPot, ItemComponent(
            SubTexture2D::CreateFromCoords(InventoryGUISystem::ItemSpritesheet, {0, 9}, {32, 32})));
        coordinator->AddComponent(healthPot, SpriteRendererComponent(
            SubTexture2D::CreateFromCoords(TransparentItemSpritesheet, {0, 17}, {32, 32})));

        auto rigidbody = RigidBody2DComponent();
        rigidbody.LinearVelocity.y = 40.0f;
        coordinator->AddComponent(healthPot, rigidbody); 
        coordinator->AddComponent(healthPot, BoxCollider2DComponent({}, {10.f, 10.f})); 
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        physics_quadtree_component.Active = true;
        coordinator->AddComponent(healthPot, physics_quadtree_component); 
        coordinator->AddComponent(healthPot, PickupComponent()); 
        coordinator->AddComponent(healthPot, HealthPotionComponent(50.f)); 

        SoundController::Instance()->GetSound(8)->SetPosition(positionOfDestroyedEntity.x, positionOfDestroyedEntity.y, 0);
        SoundController::Instance()->PlaySoundByID(8, true);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 0.9f, 0.5f, 0.5f, 1.0f };
        auto position = coordinator->GetComponent<TransformComponent>(e).Position;
        SoundController::Instance()->GetSound(7)->SetPosition(position.x, position.y, 0);
        SoundController::Instance()->PlaySoundByID(7, true);
    },
    [](Entity e)
    {
        auto& sprite_renderer = coordinator->GetComponent<SpriteRendererComponent>(e);
        sprite_renderer.Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    }));
    coordinator->AddComponent(healthPotionBoxCave, BreakableComponent(5));

#pragma region SPIKE_SETUP
    auto spike_texture = SubTexture2D::CreateFromCoords(spikeTexture_, {1.00f, 1.00f}, {16.00f, 16.00f});
    spike_texture->Inset(0.009f);

    for (int i = 0; i < 4; ++i)
    {
        auto spikeEntity = coordinator->CreateEntity();
        coordinator->AddComponent(spikeEntity, SpikeComponent(10.f));
        auto spikeRigidbody = RigidBody2DComponent();
        spikeRigidbody.BodyType = Physics::RigidBodyType::Static;
        coordinator->AddComponent(spikeEntity, TransformComponent(glm::vec3(-176 + i * 16, 42, -0.5f),
            glm::vec3(0), glm::vec3(16, 16, 1)));
        coordinator->AddComponent(spikeEntity, SpriteRendererComponent(spike_texture));
        coordinator->AddComponent(spikeEntity, spikeRigidbody); 
        coordinator->AddComponent(spikeEntity, BoxCollider2DComponent({0, -4.f}, {8.f, 3.f})); 
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        physics_quadtree_component.Active = true;
        coordinator->AddComponent(spikeEntity, physics_quadtree_component); 

    }
    
    for (int i = 0; i < 8; ++i)
    {
        auto spikeEntity = coordinator->CreateEntity();
        coordinator->AddComponent(spikeEntity, SpikeComponent(10.f));
        auto spikeRigidbody = RigidBody2DComponent();
        spikeRigidbody.BodyType = Physics::RigidBodyType::Static;
        coordinator->AddComponent(spikeEntity, TransformComponent(glm::vec3(-608 + i * -16, -22, -0.5f),
            glm::vec3(0), glm::vec3(16, 16, 1)));
        coordinator->AddComponent(spikeEntity, SpriteRendererComponent(spike_texture));
        coordinator->AddComponent(spikeEntity, spikeRigidbody); 
        coordinator->AddComponent(spikeEntity, BoxCollider2DComponent({0, -4.f}, {8.f, 3.f})); 
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        physics_quadtree_component.Active = true;
        coordinator->AddComponent(spikeEntity, physics_quadtree_component); 

    }

    for (int i = 0; i < 2; ++i)
    {
        auto spikeEntity = coordinator->CreateEntity();
        coordinator->AddComponent(spikeEntity, SpikeComponent(10.f));
        auto spikeRigidbody = RigidBody2DComponent();
        spikeRigidbody.BodyType = Physics::RigidBodyType::Static;
        coordinator->AddComponent(spikeEntity, TransformComponent(glm::vec3(-320 + i * -16, -22, -0.5f),
            glm::vec3(0), glm::vec3(16, 16, 1)));
        coordinator->AddComponent(spikeEntity, SpriteRendererComponent(spike_texture));
        coordinator->AddComponent(spikeEntity, spikeRigidbody); 
        coordinator->AddComponent(spikeEntity, BoxCollider2DComponent({0, -4.f}, {8.f, 3.f})); 
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        physics_quadtree_component.Active = true;
        coordinator->AddComponent(spikeEntity, physics_quadtree_component); 
    }

    for (int i = 0; i < 3; ++i)
    {
        auto spikeEntity = coordinator->CreateEntity();
        coordinator->AddComponent(spikeEntity, SpikeComponent(10.f));
        auto spikeRigidbody = RigidBody2DComponent();
        spikeRigidbody.BodyType = Physics::RigidBodyType::Static;
        coordinator->AddComponent(spikeEntity, TransformComponent(glm::vec3(-528 + i * -16, 106, -0.5f),
            glm::vec3(0, 0, glm::pi<float>()), glm::vec3(16, 16, 1)));
        coordinator->AddComponent(spikeEntity, SpriteRendererComponent(spike_texture));
        coordinator->AddComponent(spikeEntity, spikeRigidbody); 
        coordinator->AddComponent(spikeEntity, BoxCollider2DComponent({0, 4.f}, {8.f, 3.f})); 
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        physics_quadtree_component.Active = true;
        coordinator->AddComponent(spikeEntity, physics_quadtree_component); 
    }

    for (int i = 0; i < 4; ++i)
    {
        auto spikeEntity = coordinator->CreateEntity();
        coordinator->AddComponent(spikeEntity, SpikeComponent(10.f));
        auto spikeRigidbody = RigidBody2DComponent();
        spikeRigidbody.BodyType = Physics::RigidBodyType::Static;
        coordinator->AddComponent(spikeEntity, TransformComponent(glm::vec3(-592 + i * -16, 170, -0.5f),
            glm::vec3(0), glm::vec3(16, 16, 1)));
        coordinator->AddComponent(spikeEntity, SpriteRendererComponent(spike_texture));
        coordinator->AddComponent(spikeEntity, spikeRigidbody); 
        coordinator->AddComponent(spikeEntity, BoxCollider2DComponent({0, -4.f}, {8.f, 3.f})); 
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        physics_quadtree_component.Active = true;
        coordinator->AddComponent(spikeEntity, physics_quadtree_component); 
    }

    for (int i = 0; i < 29; ++i)
    {
        auto spikeEntity = coordinator->CreateEntity();
        coordinator->AddComponent(spikeEntity, SpikeComponent(10.f));
        auto spikeRigidbody = RigidBody2DComponent();
        spikeRigidbody.BodyType = Physics::RigidBodyType::Static;
        coordinator->AddComponent(spikeEntity, TransformComponent(glm::vec3(-784 + i * 16, 442, -0.5f),
            glm::vec3(0), glm::vec3(16, 16, 1)));
        coordinator->AddComponent(spikeEntity, SpriteRendererComponent(spike_texture));
        coordinator->AddComponent(spikeEntity, spikeRigidbody); 
        coordinator->AddComponent(spikeEntity, BoxCollider2DComponent({0, -4.f}, {8.f, 3.f})); 
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        physics_quadtree_component.Active = true;
        coordinator->AddComponent(spikeEntity, physics_quadtree_component); 
    }
#pragma endregion
#pragma endregion

#pragma region ENEMIES
    {
        auto batEntity = coordinator->CreateEntity();
        coordinator->AddComponent(batEntity, TransformComponent(glm::vec3(-60, 156, 0), glm::vec3(0), glm::vec3(16, 24, 1)));
        coordinator->AddComponent(batEntity, BoxCollider2DComponent({0, -14.f}, {2.0f, 2.f})); 
        auto rigidbody = RigidBody2DComponent();
        rigidbody.BodyType = Physics::RigidBodyType::Dynamic;
        rigidbody.GravityScale = 0.0f;
        auto physics_quadtree_component = PhysicsQuadtreeComponent();
        coordinator->AddComponent(batEntity, physics_quadtree_component); 

        auto firstFrame = SubTexture2D::CreateFromCoords(batSpritesheet_, glm::vec2(0, 1), glm::vec2(16, 24));
        SpriteRendererComponent src = SpriteRendererComponent(firstFrame);
        src.Offset.y = -16.0f;
        src.Offset.x = -2.0f;
        coordinator->AddComponent(batEntity, src);
        coordinator->AddComponent(batEntity, rigidbody);
        BatComponent bat;
        bat.FlightSpeed = 100;
        coordinator->AddComponent(batEntity, bat);

        FlyingAnimationComponent fac;
        constexpr glm::vec2 size = { 16, 24 };

        auto& faca = fac.Animation;
        for (int i = 0; i < 5; ++i)
        {
            auto texture = 
                SubTexture2D::CreateFromCoords(batSpritesheet_, glm::vec2(i , 1), size);
            // texture inset 
            texture->Inset(0.001f);

            faca.SpriteTextures.emplace_back(texture);
        }

        for (size_t i = 0; i < faca.SpriteTextures.size(); ++i)
        {
            faca.AnimationIndices.emplace_back(i);
        }

        faca.FramesBetweenTransition = 8;
        coordinator->AddComponent(batEntity, fac);
        coordinator->AddComponent(batEntity, HealthComponent(40, 20, [](Entity e)
        {
            coordinator->DestroyEntity(e);
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
        coordinator->AddComponent(batEntity, BreakableComponent(5));
    }

    {
        constexpr glm::vec2 size = { 80, 80 };
        auto nightborneEntity = coordinator->CreateEntity();
        auto firstFrame = SubTexture2D::CreateFromCoords(nightborneSpritesheet_, glm::vec2(0, 4), size);
        swingingAnimationSystem_->SetOriginalTexture(firstFrame, nightborneEntity);
        // firstFrame->Inset(0.01f);

        coordinator->AddComponent(nightborneEntity, TransformComponent(glm::vec3(-520, 300, 0), glm::vec3(), glm::vec3(size, 0)));
        SpriteRendererComponent src(firstFrame);
        coordinator->AddComponent(nightborneEntity, src);
        coordinator->AddComponent(nightborneEntity, BoxCollider2DComponent({0, -13.5f}, {10.0f, 10.0f}));

        auto rigidbody = RigidBody2DComponent();
        rigidbody.BodyType = Physics::RigidBodyType::Dynamic;
        auto physics_quadtree_component = PhysicsQuadtreeComponent();

        coordinator->AddComponent(nightborneEntity, physics_quadtree_component); 
        coordinator->AddComponent(nightborneEntity, rigidbody);

        NightborneComponent nightborne;
        nightborne.Speed = 40.f;
        coordinator->AddComponent(nightborneEntity, nightborne);

        SwingingAnimationComponent sac;

        auto& saca = sac.Animation;
        for (int i = 0; i < 12; ++i)
        {
            auto texture = 
                SubTexture2D::CreateFromCoords(nightborneSpritesheet_, glm::vec2(i , 2), size);
            // texture inset 
            texture->Inset(0.001f);

            saca.SpriteTextures.emplace_back(texture);
        }

        for (size_t i = 0; i < saca.SpriteTextures.size(); ++i)
        {
            saca.AnimationIndices.emplace_back(i);
        }

        saca.FramesBetweenTransition = 8;
        coordinator->AddComponent(nightborneEntity, sac);
        coordinator->AddComponent(nightborneEntity, HealthComponent(100, 30, [](Entity e)
        {
            coordinator->DestroyEntity(e);
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

        coordinator->AddComponent(nightborneEntity, BreakableComponent(5));

        auto melee = coordinator->CreateEntity();
        // coordinator->AddComponent(melee, SpriteRendererComponent());
        coordinator->AddComponent(melee, RigidBody2DComponent(Physics::RigidBodyType::Static));
        coordinator->AddComponent(melee, BoxCollider2DComponent({}, {8, 15}));
        PhysicsQuadtreeComponent pqc;
        pqc.Active = false;
        coordinator->AddComponent(melee, pqc);
        coordinator->AddComponent(melee, TransformComponent({}, {}, glm::vec3(8, 15, 1)));
        coordinator->AddComponent(melee, SpikeComponent(nightborne.Damage));

        coordinator->AddComponent(nightborneEntity, ReferenceComponent(melee));
    }

    {
        // constexpr glm::vec2 size = { 80, 93 };
        auto y = coordinator->CreateEntity();
        auto firstFrame = CreateRef<SubTexture2D>((CreateRef<Texture2D>("Assets/Images/Bringer-of-Death_Idle_1.png")), glm::vec2(), glm::vec2(1));
        swingingAnimationSystem_->SetOriginalTexture(firstFrame, y);
        // firstFrame->Inset(0.01f);

        coordinator->AddComponent(y, TransformComponent(glm::vec3(-90, 470, 0), glm::vec3(), glm::vec3(140, 93, 0)));
        SpriteRendererComponent src(firstFrame);
        src.Offset.x = -30; 
        coordinator->AddComponent(y, src);
        coordinator->AddComponent(y, BoxCollider2DComponent({0, -5.f}, {20.0f, 40.0f}));

        auto rigidbody = RigidBody2DComponent();
        rigidbody.BodyType = Physics::RigidBodyType::Dynamic;
        auto physics_quadtree_component = PhysicsQuadtreeComponent();

        coordinator->AddComponent(y, physics_quadtree_component); 
        coordinator->AddComponent(y, rigidbody);

        BringerComponent bringer;
        bringer.Speed = 40.f;
        coordinator->AddComponent(y, bringer);

        SwingingAnimationComponent sac;

        auto& saca = sac.Animation;
        for (int i = 1; i < 9; ++i)
        {
            auto texture = 
                CreateRef<SubTexture2D>(CreateRef<Texture2D>("Assets/Spritesheets/Cast/Bringer-of-Death_Cast_" + std::to_string(i) + ".png"), glm::vec2(), glm::vec2(1));

            if (texture) 
                texture->Inset(0.001f);
            // texture inset 

            saca.SpriteTextures.emplace_back(texture);
        }

        for (size_t i = 0; i < saca.SpriteTextures.size(); ++i)
        {
            saca.AnimationIndices.emplace_back(i);
        }

        saca.FramesBetweenTransition = 8;
        coordinator->AddComponent(y, sac);
        coordinator->AddComponent(y, HealthComponent(100, 30, [](Entity e)
        {
            coordinator->DestroyEntity(e);
            auto portalEntity3 = coordinator->CreateEntity();
            coordinator->AddComponent(portalEntity3, TransformComponent(glm::vec3(-90, 470.f, 0), glm::vec3(0), glm::vec3(30, 30, 1)));
            coordinator->AddComponent(portalEntity3, SpriteRendererComponent(
                CreateRef<SubTexture2D>(CreateRef<Texture2D>("Assets/Images/portal.png"), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f)))); 
            coordinator->AddComponent(portalEntity3, PortalComponent([](Entity e, PlayerEnterEvent& event)
            {
                GameManager::Instance()->ChangeState(GameState::Win);
            }));
            coordinator->AddComponent(portalEntity3, BoxCollider2DComponent({}, {8, 8}));
            coordinator->AddComponent(portalEntity3, RigidBody2DComponent(Physics::RigidBodyType::Static));
            coordinator->AddComponent(portalEntity3, PhysicsQuadtreeComponent());

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

        coordinator->AddComponent(y, BreakableComponent(5));

        auto melee = coordinator->CreateEntity();
        coordinator->AddComponent(melee, SpriteRendererComponent(CreateRef<SubTexture2D>(CreateRef<Texture2D>("Assets/Spritesheets/Bringer-of-Death_Spell_8.png"), glm::vec2(), glm::vec2(1))));
        coordinator->AddComponent(melee, RigidBody2DComponent(Physics::RigidBodyType::Static));
        coordinator->AddComponent(melee, BoxCollider2DComponent({}, {15, 30}));
        PhysicsQuadtreeComponent pqc;
        pqc.Active = false;
        coordinator->AddComponent(melee, pqc);
        coordinator->AddComponent(melee, TransformComponent({}, {}, glm::vec3(0, 0, 0 )));
        coordinator->AddComponent(melee, SpikeComponent(bringer.Damage));

        coordinator->AddComponent(y, ReferenceComponent(melee));
    }
#pragma endregion
}

auto MainLayer::OnDetach() -> void 
{
    coordinator->Clear();
}

auto MainLayer::OnUpdate(Timestep ts) -> void 
{
    // cameraController_.OnUpdate(ts);
    static MusicPlayer* musicPlayer = MusicPlayer::Instance();
    static GameManager* gameManager = GameManager::Instance();

    if (Input::Instance()->IsKeyPressed(Key::Backslash))
    {
        if (gameManager->GetState() == GameState::PlayingLevel)
            gameManager->ChangeState(GameState::Paused);
        else
            gameManager->ChangeState(GameState::PlayingLevel);
    }

    if (Input::Instance()->IsKeyPressed(Key::Slash))
    {
        if (gameManager->GetState() == GameState::PlayingLevel)
            gameManager->ChangeState(GameState::About);
    }

    if (!musicPlayer->GetCurrentSound()->getIsPaused())
        musicTimer_ += ts;

    if (gameManager->GetState() == GameState::PlayingLevel)
    {
        timer_ += ts;
        gameManager->UploadTime(timer_);
        physicsSystem_->tilemapSystem->Update(ts);
        weaponSystem_->Update(ts);
        damageableSystem_->Update(ts);
        physicsSystem_->Update(ts);

        playerSystem_->Update(ts);
        batSystem_->Update(ts);
        nightborneSystem_->Update(ts);
        bringerSystem_->Update(ts);

        portalSystem_->Update(ts);
        projectileSystem_->Update(ts);
        cameraController_.GetCamera().SetPosition(
            smoothCameraFollowSystem_->GetCalculatedPosition(ts));

    }

    Renderer2D::BeginScene(cameraController_.GetCamera());

    constexpr float tiling_factor = 128.0f;
    Renderer2D::DrawQuad(glm::vec3(), glm::vec2(2560, 2560), brownBackground_, tiling_factor);

    constexpr glm::vec4 background_colour = glm::vec4(135.f, 206.f, 250.f, 1.0f);
    RenderCommand::SetClearColour(Utility::Colour32BitConvert(background_colour));
    RenderCommand::Clear();

    if (gameManager->GetState() == GameState::PlayingLevel)
    {
        runningAnimationSystem_->Update(ts);
        swingingAnimationSystem_->Update(ts);
        batFlyingAnimationSystem_->Update(ts);
    } 

    tilemapRenderSystem_->Update(ts);
    spriteRenderSystem_->Update(ts);

    Renderer2D::EndScene();
}


auto MainLayer::OnEvent(Event &e) -> void 
{
#define OEB(x) std::bind(&std::remove_reference<decltype(*(x))>::type::OnEvent, x, std::placeholders::_1)
    static const std::array<std::function<void(Event&)>, 20> on_events {
        OEB(physicsSystem_),
        OEB(projectileSystem_),
        OEB(weaponSystem_),
        OEB(healingPotionSystem_),

        OEB(&cameraController_),

        OEB(breakableBoxSystem_),
        OEB(damageableSystem_),

        OEB(playerSystem_),
        OEB(batSystem_),

        OEB(pickupSystem_),
        OEB(spikeSystem_),
        OEB(portalSystem_),
        OEB(weaponAffectedByPickupSystem_),

        OEB(runningAnimationSystem_),
        OEB(swingingAnimationSystem_),
        OEB(batFlyingAnimationSystem_),
        OEB(nightborneSystem_),
        OEB(bringerSystem_),
        OEB(playerAffectedByAnimationSystem_),
        OEB(weaponAffectedByAnimationSystem_)
    };
#undef OEB 

    static GameManager* gameManager = GameManager::Instance();
    if (gameManager->GetState() != GameState::Paused)
    {
        for (auto& func : on_events)
        {
            if (e.Handled)
                return;
            func(e);
        }
    }
}

void CentreText(const char* text)
{
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text);

}

auto MainLayer::OnImGuiRender() -> void 
{
    auto io = ImGui::GetIO();

    ImGui::SetNextWindowSize(ImVec2(110, 40));
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 20, 0), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::Begin("Timer", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("%.2fs", timer_);
    ImGui::End();

    static GameManager* gm = GameManager::Instance();

    if (gm->GetState() == GameState::Paused)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 200, io.DisplaySize.y * 0.5f - 150));
        ImGui::SetNextWindowBgAlpha(0.2f);
        static MusicPlayer* player = MusicPlayer::Instance();
        ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings);
        float bgmVolume = player->GetVolume();
        ImGui::SliderFloat("BGM Master volume", &bgmVolume, 0.0f, 1.0f);
        player->SetMasterVolume(bgmVolume);
        static SoundController* sc = SoundController::Instance();
        float soundVolume = sc->GetVolume();
        ImGui::SliderFloat("SFX Master volume", &soundVolume, 0.0f, 1.0f);
        sc->SetMasterVolume(soundVolume);

        static float crossfadeDuration = 0.5f;
        
        static const char* items[] = { "My Castle Town", "The Tale of a Cruel World" };
        static const char* current_item = "My Castle Town";
        static const char* item_before;

        static bool reverb_enabled = false;

        if (ImGui::BeginCombo("Background Music", current_item)) // The second parameter is the label previewed before opening the combo.
        {
            item_before = current_item;
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(items[n], is_selected))
                {
                    current_item = items[n];
                    if (item_before != current_item)
                    {
                        player->AddTransition(CreateScope<FadeOutTransition>(player->GetCurrentSound(), crossfadeDuration));
                        player->PlayMusicByID(player->GetID(std::string(current_item)), !isPlayingMusic_);
                        musicTimer_ = 0.f;
                        player->AddTransition(CreateScope<FadeInTransition>(player->GetCurrentSound(), crossfadeDuration));
                    }

                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
            ImGui::EndCombo();
        }

        ISoundEffectControl* sfxCtrl = player->GetCurrentSound()->getSoundEffectControl();
        // if (player->GetCurrentSound())
        // {
        //     sfxCtrl = player->GetCurrentSound()->getSoundEffectControl();
        // }
        // else
        // {
        //     sfxCtrl = nullptr;
        // }

        ImGui::Checkbox("Enable reverb for music", &reverb_enabled);
        if (sfxCtrl)
        {
            if (reverb_enabled)
            {
                if (!sfxCtrl->isI3DL2ReverbSoundEffectEnabled())
                {
                    sfxCtrl->enableI3DL2ReverbSoundEffect();
                }
            }
            else
            {
                if (sfxCtrl->isI3DL2ReverbSoundEffectEnabled())
                {
                    sfxCtrl->disableI3DL2ReverbSoundEffect();
                }
            }
        }
        else
        {
            CC_TRACE("Null");
        }

        ImGui::SliderFloat("Crossfade length", &crossfadeDuration, 0.0f, 10.f);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50.f);
        ImGui::SetCursorPosX(200 - 16);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
            if (isPlayingMusic_)
            {
                if (ImGui::ImageButton(Utility::ImGuiImageTexture(pauseButton_), {32, 32}, 
                    ImVec2(pauseButton_->GetTexCoordsArray()[0].x, pauseButton_->GetTexCoordsArray()[0].y),
                    ImVec2(pauseButton_->GetTexCoordsArray()[2].x, pauseButton_->GetTexCoordsArray()[2].y)
                ))
                {
                    player->AddTransition(CreateScope<FadeOutTransition>(player->GetCurrentSound(), 0.25f, 0.0f, [](ISound* sound){
                        sound->setIsPaused(true);
                    }));
                    isPlayingMusic_ = false;
                }
            }
            else
            {
                if (ImGui::ImageButton(Utility::ImGuiImageTexture(playButton_), {32, 32}, 
                    ImVec2(playButton_->GetTexCoordsArray()[0].x, playButton_->GetTexCoordsArray()[0].y),
                    ImVec2(playButton_->GetTexCoordsArray()[2].x, playButton_->GetTexCoordsArray()[2].y)
                ))
                {
                    player->AddTransition(CreateScope<FadeInTransition>(player->GetCurrentSound(), 0.25f));
                    player->SetPause(false);
                    isPlayingMusic_ = true;
                }
            }
        ImGui::PopStyleVar();

        float time = musicTimer_ * 1000.f;
        if (time > static_cast<float>(player->GetCurrentSound()->getPlayLength()))
        {
            musicTimer_ = 0.f;
        }

        float value = time / static_cast<float>(player->GetCurrentSound()->getPlayLength());
        ImGui::SetNextItemWidth(250.f);
        ImGui::SetCursorPosX(200.f - 125.f);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 0));
            ImGui::SliderFloat(" ", &value, 0.f, 1.0f, "%.2f", ImGuiSliderFlags_NoInput);
        ImGui::PopStyleColor();

        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth   = ImGui::CalcTextSize(current_item).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(current_item);


        ImGui::End();
    }
    else if (gm->GetState() == GameState::Win)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 200, io.DisplaySize.y * 0.5f - 150));
        ImGui::SetNextWindowBgAlpha(0.2f);
        ImGui::Begin("Win", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings);
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth   = ImGui::CalcTextSize("You win!").x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("You win!");
        ImGui::SetCursorPosX(200.f - 75.f);
        if (ImGui::Button("Menu", ImVec2(150.f, 100.f)))
        {
            GameManager::Instance()->ChangeState(GameState::MenuLevel);
        }
        ImGui::End();
    }
    else if (gm->GetState() == GameState::About)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 200, io.DisplaySize.y * 0.5f - 150));
        ImGui::SetNextWindowBgAlpha(0.2f);
        ImGui::Begin("About", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings);

        CentreText("About this game.");
        CentreText("A and D to move left and right respectively.");
        CentreText("S to drop down from platforms.");
        CentreText("Space to jump.");
        CentreText("F to dash.");
        CentreText("Backslash to open up settings menu.");

        ImGui::SetCursorPosX(200.f - 75.f);
        if (ImGui::Button("Close", ImVec2(150.f, 100.f)))
        {
            GameManager::Instance()->ChangeState(GameState::PlayingLevel);
        }

        ImGui::End();
    }

    inventoryGUISystem_->OnImGuiRender();
    playerHealthGUISystem_->OnImGuiRender();
}
