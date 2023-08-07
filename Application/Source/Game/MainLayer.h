#include "Core/Layer.h"
#include "Scene/Scene.h"
#include "Core/Core.h"

#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/PlayerSystem.h"
#include "ECS/Systems/BatSystem.h"
#include "ECS/Systems/GameSystem.h"
#include "ECS/Systems/GUISystem.h"

#include "OrthographicCameraController.h"
#include "Renderer/Texture.h"
#include "Tilemap.h"

#include "Physics/PhysicsSystem.h"

class MainLayer : public Layer
{
public:
    MainLayer();
    virtual ~MainLayer() = default;

    virtual auto OnAttach() -> void override;
    virtual auto OnDetach() -> void override;

    auto OnUpdate(Timestep ts) -> void override;
    auto OnEvent(Event& e) -> void override;
    auto OnImGuiRender() -> void override;

private:
    float timer_ = 0.f;
    float musicTimer_ = 0.f;
    bool isPlayingMusic_ = true;

    OrthographicCameraController cameraController_;

    Ref<Texture2D> buttons_;
    Ref<SubTexture2D> playButton_;
    Ref<SubTexture2D> pauseButton_;

    Ref<Texture2D> batSpritesheet_;

    Ref<Texture2D> nareLogoTexture_;
    Ref<Scene> activeScene_;

    Ref<Texture2D> itemSpritesheet_;
    Ref<SubTexture2D> swordSprite_;
    Ref<SubTexture2D> emptyItemSprite_;

    // Background
    Ref<Texture2D> blueBackground_;
    Ref<Texture2D> brownBackground_;

    // Spritesheet
    Ref<Texture2D> terrainSpritesheet_;
    Ref<Texture2D> playerSpritesheet_;
    static Ref<Texture2D> TransparentItemSpritesheet;
    Ref<Texture2D> spikeTexture_;
    
    // Render Systems 
    Ref<SpriteRenderSystem> spriteRenderSystem_;
    Ref<TilemapRenderSystem> tilemapRenderSystem_;
    Ref<SmoothCameraFollowSystem> smoothCameraFollowSystem_;

    // Animation systems
    Ref<RunningAnimationSystem> runningAnimationSystem_;
    Ref<SwingingAnimationSystem> swingingAnimationSystem_;
    Ref<BatFlyingAnimationSystem> batFlyingAnimationSystem_;

    // Physics System
    Ref<PhysicsSystem> physicsSystem_;

    // Game Logic systems
    Ref<BatSystem> batSystem_;
    Ref<PlayerSystem> playerSystem_;
    Ref<DamageableSystem> damageableSystem_;
    Ref<PlayerAffectedByAnimationSystem> playerAffectedByAnimationSystem_;
    Ref<WeaponAffectedByAnimationSystem> weaponAffectedByAnimationSystem_;
    Ref<WeaponSystem> weaponSystem_;
    Ref<PickupItemSystem> pickupSystem_;
    Ref<WeaponAffectedByPickupSystem> weaponAffectedByPickupSystem_;
    Ref<BreakableBoxSystem> breakableBoxSystem_;
    Ref<SpikeSystem> spikeSystem_;
    Ref<HealingPotionSystem> healingPotionSystem_;
    Ref<PortalSystem> portalSystem_;
    Ref<ProjectileSystem> projectileSystem_;

    // GUI Systems
    Ref<InventoryGUISystem> inventoryGUISystem_;
    Ref<PlayerHealthGUISystem> playerHealthGUISystem_;

    // Tilemaps
    Ref<TilemapComponent> testTilemap_;
};