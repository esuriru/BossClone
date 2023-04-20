#pragma once
#include <memory>
#include "TerrainCollider.h"
#include "Renderer/Mesh.h"
#include "Scene/GameObject.h"

constexpr int TERRAIN_SIZE = 800;

class Terrain : public GameObject
{
public:
	static std::shared_ptr<Terrain> GenerateTerrain(const std::string& height_map, const std::string& background_texture);

	static std::shared_ptr<Terrain> GenerateTerrain(
		const std::string& height_map,
		const std::string& background_texture,
		const std::string& blend_map,
		const std::string& r_texture,
		const std::string& g_texture,
		const std::string& b_texture,
		float bump_multiplier = 1.0f
		);
	
	~Terrain();
	Terrain();

	float GetHeight(float x, float z) const;
private:
	TerrainCollider* collider_;


};

