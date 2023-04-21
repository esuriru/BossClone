#include "MainLayer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"

MainLayer::MainLayer()
    : Layer("Main")
    , cameraController_(1280.0f/ 720.0f)
{
    this->nareLogoTexture_ = CreateRef<Texture2D>("Assets/Images/Nare Logo.png");
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
    Renderer2D::DrawQuad({0, 0, 0}, {20, 20}, { 0.8f, 0.2f, 0.2f, 1.0f });
    Renderer2D::DrawQuad({0, 20, 0}, {10, 10}, nareLogoTexture_);

    Renderer2D::EndScene();
}

auto MainLayer::OnEvent(Event &e) -> void 
{
    cameraController_.OnEvent(e);
}
