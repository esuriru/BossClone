#pragma once

#include "Core/Layer.h"
#include "OrthographicCameraController.h"
#include "Scene/Scene.h"
#include "Renderer/Texture.h"

class SceneLayer : public Layer
{
public:
    SceneLayer();
    virtual ~SceneLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;
    void OnImGuiRender() override;

private:
    OrthographicCameraController cameraController_;
    Ref<Scene> activeScene_;
};