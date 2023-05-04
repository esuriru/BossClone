#include "Core/Layer.h"
#include "Scene/Scene.h"
#include "Core/Core.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/PlayerSystem.h"
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
    Ref<Texture2D> playerIdleSpritesheet_;
    
    // Systems
    Ref<SpriteRenderSystem> spriteRenderSystem_;
    Ref<TilemapRenderSystem> tilemapRenderSystem_;
    Ref<PhysicsSystem> physicsSystem_;
    Ref<PlayerSystem> playerSystem_;

    // Animation systems
    Ref<RunningAnimationSystem> runningAnimationSystem_;

    // Tilemaps
    Ref<TilemapComponent> testTilemap_;
};