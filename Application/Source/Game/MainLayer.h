#include "Core/Layer.h"
#include "Scene/Scene.h"
#include "Core/Core.h"

#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/PlayerSystem.h"
#include "ECS/Systems/GameSystem.h"

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

private:
    OrthographicCameraController cameraController_;

    Ref<Texture2D> nareLogoTexture_;
    Ref<Scene> activeScene_;

    // Spritesheet
    Ref<Texture2D> terrainSpritesheet_;
    Ref<Texture2D> playerSpritesheet_;
    
    // Render Systems 
    Ref<SpriteRenderSystem> spriteRenderSystem_;
    Ref<TilemapRenderSystem> tilemapRenderSystem_;

    // Animation systems
    Ref<RunningAnimationSystem> runningAnimationSystem_;
    Ref<SwingingAnimationSystem> swingingAnimationSystem_;

    // Physics System
    Ref<PhysicsSystem> physicsSystem_;

    // Game Logic systems
    Ref<PlayerSystem> playerSystem_;
    Ref<DamageableSystem> damageableSystem_;
    Ref<PlayerAffectedByAnimationSystem> playerAffectedByAnimationSystem_;
    Ref<WeaponAffectedByAnimationSystem> weaponAffectedByAnimationSystem_;


    Ref<WeaponSystem> weaponSystem_;

    // Tilemaps
    Ref<TilemapComponent> testTilemap_;
};