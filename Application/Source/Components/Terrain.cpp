#include "Terrain.h"
#include "TerrainVolume.h"
#include "MyMath.h"
#include "Renderer/Shader.h"
#include "Renderer/MeshBuilder.h"
#include "Renderer/Texture.h"
#include "stb_image.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"

constexpr int MAX_HEIGHT = 40;
float bump_height = MAX_HEIGHT * 1.0f;
constexpr int MAX_PIXEL_COLOUR = 256 * 256 * 256;
constexpr float HALF_MAX_PIXEL_COLOUR = MAX_PIXEL_COLOUR * 0.5f;
const float INVERSE_HALF_MAX_PIXEL_COLOUR = 1.f / HALF_MAX_PIXEL_COLOUR;


float get_height(int x, int z, int width, int height, uint32_t channelCount, unsigned char* data)
{
	if (x < 0 || x >= height || z < 0 || z >= height)
		return 0;

	unsigned bytePerPixel = channelCount;
	unsigned char* pixelOffset = data + (x + width * z) * bytePerPixel;
	unsigned char r = pixelOffset[0];
	unsigned char g = pixelOffset[1];
	unsigned char b = pixelOffset[2];
	unsigned char a = channelCount >= 4 ? pixelOffset[3] : 0xff;

	float vheight = static_cast<float>(r) * static_cast<float>(g) * static_cast<float>(b);
	vheight += HALF_MAX_PIXEL_COLOUR;
	vheight *= INVERSE_HALF_MAX_PIXEL_COLOUR;
	vheight *= bump_height;
	return vheight;
}

glm::vec3 CalculateNormal(int x, int z, int width, int height, uint32_t channelCount, unsigned char* data)
{
	float hL = get_height(x - 1, z, width, height, channelCount, data);
	float hR = get_height(x + 1, z, width, height, channelCount, data);
	float hD = get_height(x, z - 1, width, height, channelCount, data);
	float hU = get_height(x, z + 1, width, height, channelCount, data);

	glm::vec3 normal = { hL - hR, 2.f, hD - hU };
	return glm::normalize(normal);
}


std::shared_ptr<Terrain> Terrain::GenerateTerrain(const std::string& height_map, const std::string& background_texture)
{
    throw std::logic_error("Not implemented.");
    return nullptr;
}

std::shared_ptr<Terrain> Terrain::GenerateTerrain(const std::string& height_map, const std::string& background_texture, const std::string& blend_map, const std::string& r_texture, const std::string& g_texture, const std::string& b_texture, float bump_multiplier)
{
	int width, height, channels;

	// Flip the image
	stbi_set_flip_vertically_on_load(1);
	auto data = stbi_load(height_map.data(), &width, &height, &channels, 0);

	if (!data)
	{
		std::cout << "Failed to load image!\n";
		return nullptr;
	}
	
	const int vert_count = height;
	const int count = vert_count * vert_count;
	
	std::vector<Vertex> vertex_buffer_data;
	vertex_buffer_data.reserve(height * height);
	//std::vector<uint32_t> indices;
	std::vector<uint32_t> indices;
	indices.reserve((vert_count - 1) * (vert_count - 1) * 6);

	std::vector<std::vector<float>> heights(vert_count, std::vector<float>(vert_count));
	std::vector<std::vector<glm::vec3>> normals(vert_count, std::vector<glm::vec3>(vert_count));
		
	bump_height = bump_multiplier * MAX_HEIGHT;

	int vertPointer = 0;
	for (int i = 0; i < vert_count; ++i)
	{
		for (int j = 0; j < vert_count; ++j)
		{
			vertex_buffer_data.push_back(Vertex());
			float v_height = get_height(j, i, width, height, channels, data);
			heights[j][i] = v_height;
			vertex_buffer_data.back().pos = {static_cast<float>(j) / (static_cast<float>(vert_count) - 1) * TERRAIN_SIZE, v_height, static_cast<float>(i) / (static_cast<float>(vert_count) - 1) * TERRAIN_SIZE};
			vertex_buffer_data.back().normal = CalculateNormal(j, i, width, height, channels, data);
			normals[j][i] = vertex_buffer_data.back().normal;
			vertex_buffer_data.back().texCoord = { static_cast<float>(j) / (static_cast<float>(vert_count) - 1),static_cast<float>(i) / (static_cast<float>(vert_count) - 1) };
			//++vertPointer;
		}
	}

	int pointer = 0;
	for (int gz = 0; gz < vert_count - 1; ++gz)
	{
		for (int gx = 0; gx < vert_count - 1; ++gx)
		{
			int topLeft = (gz * vert_count) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * vert_count) + gx;
			int bottomRight = bottomLeft + 1;

			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}

    auto terrainMesh = MeshBuilder::Generate("Terrain", vertex_buffer_data, indices);
	if (!terrainMesh)
		return nullptr;

	std::shared_ptr<Terrain> temp = std::make_shared<Terrain>();
	//temp->inverseHeightsRowMinusOne = static_cast<float>(heights.size()) - 1.0f;
    temp->AddComponent<MeshFilter>()->SetMesh(terrainMesh);
	temp->AddComponent<MeshRenderer>();
	auto terrainCollider = temp->AddComponent<TerrainCollider>();
	auto terrainVolume = dynamic_cast<TerrainVolume*>(terrainCollider->boundingVolume);
	terrainCollider->volume_ = terrainVolume;
	temp->collider_ = terrainCollider;
	if (!terrainVolume)
		return nullptr;

	std::swap(heights, terrainVolume->heights_);
	std::swap(normals, terrainVolume->normals_);
	terrainVolume->inverseHeightsRowMinusOne = 1 / (static_cast<float>(terrainVolume->heights_.size()) - 1.0f);
	
	terrainMesh->materials.push_back(new Material());
	auto& mat = terrainMesh->materials.back();
	mat->materialShader = Shader::MultitexturedShader;

	//mat->textureMaps[Material::DIFFUSE] = Texture::LoadTexture(background_texture.data());
	mat->textureMaps[Material::DIFFUSE] = Texture::LoadTexture(background_texture.data());

	mat->rgbTextures[Material::R] = Texture::LoadTexture(r_texture.data());
	mat->rgbTextures[Material::G] = Texture::LoadTexture(g_texture.data());
	mat->rgbTextures[Material::B] = Texture::LoadTexture(b_texture.data());
	mat->rgbTextures[Material::BLEND] = Texture::LoadTexture(blend_map.data());

	return temp;
}

Terrain::~Terrain()
{
}

float Terrain::GetHeight(float x, float z) const
{
	return collider_->FindHeight(x, z);
}

Terrain::Terrain()
{
}



