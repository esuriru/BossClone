#pragma once

#include "Scene.h"
#include "Renderer/Mesh.h"
#include "Renderer/SceneRenderer.h"
#include "Renderer/ShaderLibrary.h"
#include "Components/Light.h"

class PoolTableScene : public Scene
{
public:
	PoolTableScene();
	virtual ~PoolTableScene();

	virtual void Init() override;
	virtual void Update(double dt) override;
	virtual void Render() override;
	virtual void Exit() override;

private:

	Mesh* ground_;
	Mesh* balls_[15];
	Mesh* cueBall;
	Mesh* cueStick;
	Mesh* walls_;
	Mesh* curvedWall_;

	// Shader Library
	ShaderLibrary* shaderLib_ = ShaderLibrary::GetInstance();

	// Renderer 
	SceneRenderer* renderer_ = SceneRenderer::GetInstance();

	std::vector<LightData*> lights_{};
};