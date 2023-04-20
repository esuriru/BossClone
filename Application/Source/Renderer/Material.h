#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <array>

#include "Shader.h"
#include "Texture.h"


struct MaterialData
{
	glm::vec4 _ambient;
	glm::vec4 _diffuse;
	glm::vec4 _specular;
	float _shininess;
	float padding4[3];

	inline ~MaterialData()
	{
		CC_TRACE("Material destroyed.");
	}
};

struct Material
{
	// Map types should probably be a global thing.
	enum MapType
	{
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		NORMAL,
		NUM_MAP_TYPES,
	};

	static inline std::string GetUniformNameFromMapType(MapType type)
	{
		switch (type)
		{
		case AMBIENT:
			return "u_ambient_texture";
		case DIFFUSE:
			return "u_diffuse_texture";
		case SPECULAR:
			return "u_specular_texture";
		case NORMAL:
			return "u_normal_texture";
		default:
			break;
		}

		std::cout << "Empty string returned\n";
		return std::string();
	}

	enum TextureType
	{
		R = 0,
		G,
		B,
		BLEND,
		NUM_TEXTURE_TYPES
	};


	static inline std::string GetUniformNameFromTextureType(TextureType type)
	{
		switch (type)
		{
		case R:
			return "u_rTex";
		case G:
			return "u_gTex";
		case B:
			return "u_bTex";
		case BLEND:
			return "u_blendMap";
		default:
			break;
		}

		CC_FATAL("Empty string returned");
		return std::string();
	}


	Shader* materialShader;
	MaterialData* data;
	//Texture* textureMaps[Material::DIFFUSE] = nullptr;

	std::array<Texture*, NUM_TEXTURE_TYPES> rgbTextures{};

	std::array<Texture*, NUM_MAP_TYPES> textureMaps{};

	int illuminationModel = 1;

	float tilingFactor = 1.0f;

	float bumpMultiplier;

	uint32_t size = 0;

	Material(const glm::vec4& c);

	Material(const glm::vec4& c, Shader* shader);
	Material(const glm::vec4& a, const glm::vec4& d, const glm::vec4& s, const float shininess, Shader* shader);
	Material();
	~Material();
};

