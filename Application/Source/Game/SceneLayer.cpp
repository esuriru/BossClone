#include "SceneLayer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/RenderCommand.h"
#include "Utils/Util.h"
#include "Scenes/PlayScene.h"
#include "Scene/SceneManager.h"
#include "Physics/PhysicsWorld.h"

SceneLayer::SceneLayer()
    : Layer("SceneLayer")
    , cameraController_(1280.0f/ 720.0f)
{
    activeScene_ = SceneManager::Instance()->CreateScene<PlayScene>();
    SceneManager::Instance()->SetActiveScene(activeScene_);
}

void SceneLayer::OnAttach() 
{
    activeScene_->Start();
    cameraController_.SetZoomLevel(125);
}

void SceneLayer::OnDetach()
{
}

void SceneLayer::OnUpdate(Timestep ts)
{
    constexpr float step = 1 / CC_FIXED_UPDATE_FRAME_RATE;
    static float accumulator = 0.f;

    accumulator += glm::min(static_cast<float>(ts), 0.25f);

    while (accumulator >= step)
    {
        activeScene_->FixedUpdate(step);
        PhysicsWorld::Instance()->FixedUpdate(step);
        accumulator -= step;
    }

    cameraController_.OnUpdate(ts);

    activeScene_->Update(ts);
    Renderer2D::BeginScene(cameraController_.GetCamera());
    constexpr glm::vec4 background_colour = glm::vec4(135.f, 206.f, 250.f, 1.0f);
    RenderCommand::SetClearColour(Utility::Colour32BitConvert(background_colour));
    RenderCommand::Clear();

    activeScene_->Render();

    Renderer2D::EndScene();
}

void SceneLayer::OnEvent(Event &e)
{
    cameraController_.OnEvent(e);
}

void SceneLayer::OnImGuiRender()
{
}
