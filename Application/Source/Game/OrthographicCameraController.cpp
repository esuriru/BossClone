#include "OrthographicCameraController.h"
#include "Utils/Input.h"
#include "Core/KeyCodes.h"
#include "Events/EventDispatcher.h"

#include "glm/gtx/string_cast.hpp"

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotationEnabled)
    : camera_(-aspectRatio * zoomLevel_, aspectRatio * zoomLevel_, -zoomLevel_, zoomLevel_)
    , rotationEnabled_(rotationEnabled)
    , aspectRatio_(aspectRatio)
{
}

auto OrthographicCameraController::OnUpdate(Timestep ts) -> void
{
    static Input* input = Input::Instance();
    if (input->IsKeyDown(Key::A))
        cameraPosition_.x -= cameraTranslationSpeed_ * ts;        
    else if (input->IsKeyDown(Key::D))
        cameraPosition_.x += cameraTranslationSpeed_ * ts;        

    if (input->IsKeyDown(Key::W))
        cameraPosition_.y += cameraTranslationSpeed_ * ts;        
    else if (input->IsKeyDown(Key::S))
        cameraPosition_.y -= cameraTranslationSpeed_ * ts;        

    if (rotationEnabled_)
    {
        if (input->IsKeyDown(Key::Q))
            cameraRotation_ += cameraRotationSpeed_ * ts;
        else if (input->IsKeyDown(Key::E))
            cameraRotation_ -= cameraRotationSpeed_ * ts;

        camera_.SetRotation(cameraRotation_);
    }
    camera_.SetPosition(cameraPosition_);
}

auto OrthographicCameraController::OnEvent(Event &e) -> void
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(CC_BIND_EVENT_FUNC(OrthographicCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(CC_BIND_EVENT_FUNC(OrthographicCameraController::OnWindowResized));
}

auto OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent &e) -> bool
{
    zoomLevel_ -= e.GetOffset().y;
    camera_.SetProjection(-aspectRatio_ * zoomLevel_, aspectRatio_ * zoomLevel_, -zoomLevel_, zoomLevel_);
    CC_TRACE("Zoom Level: ", zoomLevel_);
    return false;
}

auto OrthographicCameraController::OnWindowResized(WindowResizeEvent &e) -> bool
{
    aspectRatio_ = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());
    camera_.SetProjection(-aspectRatio_ * zoomLevel_, aspectRatio_ * zoomLevel_, -zoomLevel_, zoomLevel_);
    CC_TRACE("Aspect Ratio: ", aspectRatio_);
    return false;
}
