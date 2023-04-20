#pragma once 

#include <iostream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <unordered_map>
#include "Core/Core.h"


enum ShaderFlag
{
	None = 0,
	NORMAL_MAPPED = BIT(0), // 0 << x
	AMBIENT_MAPPED = BIT(1), // 1 << x
	DIFFUSE_MAPPED = BIT(2),
	SPECULAR_MAPPED = BIT(3),
	ENVIRONMENT_MAPPED = BIT(4),
	MULTI_TEXTURING = BIT(5),
};

static std::string GetUniformNameFromShaderFlag(int flag)
{
	switch (flag)
	{
	case NORMAL_MAPPED:
		return "u_normalMap";
	case AMBIENT_MAPPED:
		return "u_ambientMap";
	case DIFFUSE_MAPPED:
		return "u_diffuseMap";
	case SPECULAR_MAPPED:
		return "u_specularMap";
	case ENVIRONMENT_MAPPED:
		return "skybox";

	default:
		break;
	}

	CC_ERROR("Empty string returned from GetUniformNameFromShaderFlag\n");
	return std::string();
}

class Shader
{
public:
	Shader(const char* v, const char* f);
	~Shader();

	void Bind();
	void Unbind();

	inline int GetFlags() const
	{
		return flags_;
	}

	inline bool HasFlag(const ShaderFlag flag) const
	{
		return flags_ & flag;
	}

	inline void AddFlag(const ShaderFlag flag)
	{
		flags_ = flags_ | flag;
	}

	inline void SetFlags(int flags)
	{
		flags_ = flags;
	}

	void SetMat4(const std::string& name, const glm::mat4& mat);

	void SetFloat(const std::string& name, const float& val);
	void SetFloat2(const std::string& name, const glm::vec2& vec);
	void SetFloat3(const std::string& name, const glm::vec3& vec);
	void SetFloat4(const std::string& name, const glm::vec4& vec);

	void SetInt(const std::string& name, const int& val);
	void SetIntArray(const std::string& name, int* values, uint32_t count);

	static Shader* DefaultShader;
	static Shader* MappedShader;
	static Shader* MultitexturedShader;
	static Shader* BillboardShader;

	bool lit = false;

private:
	friend class UniformBuffer;
	GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

	bool loadedInLibrary_ = false;


	uint32_t rendererID_;

	std::unordered_map<std::string, GLint> shaderLocationCache_;

	std::pair<bool, GLint> TryGetLocation(const std::string& name);

	friend class ShaderLibrary;

	int flags_;

};

