#include "Core/Layer.h"
#include "Scene/Scene.h"
#include "Core/Core.h"
#include "ECS/Systems/RenderSystem.h"
#include "OrthographicCameraController.h"
#include "Renderer/Texture.h"

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
    Ref<SpriteRenderSystem> spriteRenderSystem;
};