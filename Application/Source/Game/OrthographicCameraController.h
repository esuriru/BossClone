#pragma once

#include "Renderer/OrthographicCamera.h"
#include "Core/Timestep.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"

class OrthographicCameraController
{
public:
    OrthographicCameraController(float aspectRatio, bool rotation = true);

    auto OnUpdate(Timestep ts) -> void;
    auto OnEvent(Event& e) -> void;

    inline auto GetCamera() -> const OrthographicCamera& 
    {
        return camera_;
    }
private:
    // Callbacks
    auto OnMouseScrolled(MouseScrolledEvent& e) -> bool;
    auto OnWindowResized(WindowResizeEvent& e) -> bool;

    float aspectRatio_ = 0.f;
    float zoomLevel_ = 1.0f;

    bool rotationEnabled_ = false;

    glm::vec3 cameraPosition_{};
    float cameraRotation_ = 0.f;

    float cameraTranslationSpeed_ = 25.f, cameraRotationSpeed_ = 25.f;

    OrthographicCamera camera_;
};