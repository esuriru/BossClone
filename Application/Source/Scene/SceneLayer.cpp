#include "SceneLayer.h"

SceneLayer::SceneLayer()
	: Layer("SceneLayer")
{
}

SceneLayer::~SceneLayer()
{
}

void SceneLayer::OnAttach()
{
}

void SceneLayer::OnDetach()
{
}

void SceneLayer::OnUpdate(Timestep ts)
{
	if (!scene)
		return;

	scene->Update(ts.GetSeconds());
	scene->Render();
}

void SceneLayer::OnEvent(Event& event)
{
}
