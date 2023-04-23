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

    // for (int i = 0; i < 5; ++i)
    // {
    //     auto entity = coordinator->CreateEntity();  
    //     coordinator->AddComponent(entity, TransformComponent {
    //         glm::vec3(30.f + 10.f * i, 0, 0),
    //     });

    //     coordinator->AddComponent(entity, SpriteRendererComponent());
    // }

    testTilemap_ = CreateRef<Tilemap>();
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
    // Renderer2D::DrawQuad({0, 0, 0}, {20, 20}, { 0.8f, 0.2f, 0.2f, 1.0f });
    // Renderer2D::DrawQuad({0, 20, 0}, {10, 10}, nareLogoTexture_);

    spriteRenderSystem_->Update(ts);
    Renderer2D::EndScene();
}

auto MainLayer::OnEvent(Event &e) -> void 
{
    cameraController_.OnEvent(e);
}
