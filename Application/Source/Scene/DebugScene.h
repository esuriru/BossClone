#pragma once
#include "Scene/Scene.h"
#include "Renderer/Material.h"
#include "Components/Light.h"
#include "Renderer/Mesh.h"

class DebugScene : public Scene
{
public:
	DebugScene();
	virtual ~DebugScene() override;

	virtual void Init() override;
	virtual void Update(double dt) override;
	virtual void Render() override;
	virtual void Exit() override;

	virtual Light* CreateLight();

private:
	std::unique_ptr<Material> debugMat_;
	std::unique_ptr<Material> objectMaterial_;
	std::vector<LightData*> lights_;

	Mesh* testMesh_ = nullptr;
	Mesh* cubeMesh_;
	Mesh* cubeMesh2_;
	Mesh* sphereMesh_;
	Mesh* sphereMesh2_;

	
};

