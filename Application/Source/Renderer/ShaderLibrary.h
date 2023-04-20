#pragma once
#include <array>
#include "Shader.h"
#include "UniformBuffer.h"
#include "Singleton.h"

#define MAX_ILLUM_MODELS 3 // TODO - Support '3' for now

class ShaderLibrary : public Singleton<ShaderLibrary>
{
public:
	ShaderLibrary() = default;
	~ShaderLibrary();
	void Add(const std::string& name, Shader* shader);

	Shader* Load(const std::string& name, const char* v, const char* f);

	Shader* Get(const std::string& name);

	UniformBuffer* RegisterUniformBuffer(const std::string& ubo_name);
	UniformBuffer* GetUniformBuffer(const std::string& ubo_name);

	static Shader* GetShaderFromIllumModel(int model);

private:
	void UpdateUniformBufferBindings();

	static std::array<Shader*, MAX_ILLUM_MODELS> illumModelShaders_;

	bool Exists(const std::string& name) const;

	std::unordered_map<std::string, Shader*> shaders_;
	std::map<std::string, UniformBuffer*> uniformBuffers_;

	friend class Application;
};

