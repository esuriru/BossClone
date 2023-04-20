#pragma once

#include <array>

#include "Scene.h"
#include "Renderer/Mesh.h"
#include "Renderer/SceneRenderer.h"
#include "Renderer/ShaderLibrary.h"

constexpr uint32_t MAX_LIGHTS = 8;

class ArcadeScene : public Scene
{
public:

	// Constructor
	ArcadeScene();

	// Destructor
	virtual ~ArcadeScene() override;

	// Init function
	virtual void Init() override;

	// Update function
	virtual void Update(double dt) override;

	// Render function
	virtual void Render() override;

	// Exit function
	virtual void Exit() override;

	// Create a new Light GO
	virtual Light* CreateLight();
private:
	// Enum for meshes
	enum MESHLIST
	{
		GEO_WALL = 0,
		GEO_CEILING,
		GEO_FLOOR,
		GEO_PORTAL,
		GEO_SPHERE,
		TOTAL_MESHES,
	};

	// Claw machine will have sphere, cube

	// Array of meshes
	Mesh* meshlist_[TOTAL_MESHES];
	
	// Shader Library
	ShaderLibrary* shaderLib_ = ShaderLibrary::GetInstance();

	// Renderer 
	SceneRenderer* renderer_ = SceneRenderer::GetInstance();

	std::vector<LightData*> lights_{};
	std::vector<Material*> mats_;

	float lightY;
};