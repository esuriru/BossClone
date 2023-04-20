#pragma once
#include "Core/Layer.h"
#include <memory>
#include "Scene.h"

class SceneLayer : public Layer
{
public:
	std::shared_ptr<Scene> scene;

	SceneLayer();
	virtual ~SceneLayer();
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Timestep ts) override;
	void OnEvent(Event& event) override;


};

