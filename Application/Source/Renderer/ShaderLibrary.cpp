#include "ShaderLibrary.h"

#include <iostream>

std::array<Shader*, MAX_ILLUM_MODELS> ShaderLibrary::illumModelShaders_; 

ShaderLibrary::~ShaderLibrary()
{
	for (auto& i : shaders_)
		if (i.second)
		{
			if (i.second->loadedInLibrary_)
			{
				delete i.second;
				i.second = nullptr;
			}
		}

	for (auto& i : uniformBuffers_)
	{
		if (i.second)
		{
			delete i.second;
		}
	}
}

void ShaderLibrary::Add(const std::string& name, Shader* shader)
{
	if (!shader)
	{
		std::cout << "Shader was null.";
		return;
	}

	if (Exists(name))
	{
		std::cout << "Shader already exists in the library.";
		return;
	}

	shaders_[name] = shader;

	UpdateUniformBufferBindings();
}

Shader* ShaderLibrary::Load(const std::string& name, const char* v, const char* f)
{
	auto shader = new Shader(v, f);
	Add(name, shader);
	shader->loadedInLibrary_ = true;
	return shader;
}

Shader* ShaderLibrary::Get(const std::string& name)
{
	if (!Exists(name))
	{
		CC_ERROR("Shader does not exist in the library.");
		return nullptr;
	}

	return shaders_[name];
}

UniformBuffer* ShaderLibrary::RegisterUniformBuffer(const std::string& ubo_name)
{
	const auto& preliminary_ubo_find =  uniformBuffers_.find(ubo_name);
	if (preliminary_ubo_find != uniformBuffers_.end())
	{
		CC_WARN("Uniform buffer already exists when trying to register ", ubo_name, ". Returning existing one.");
		return preliminary_ubo_find->second;
	}

	const auto& newUniformBuffer = UniformBuffer::CreateUniformBuffer(ubo_name, shaders_, uniformBuffers_.size());
	if (!newUniformBuffer)
	{
		CC_ERROR("Uniform buffer could not be registered.");
		return nullptr;
	}

				
	uniformBuffers_[ubo_name] = newUniformBuffer;
	return newUniformBuffer;
}

UniformBuffer* ShaderLibrary::GetUniformBuffer(const std::string& ubo_name)
{
	const auto& i = uniformBuffers_.find(ubo_name);
	if (i == uniformBuffers_.end())
	{
		CC_ERROR("Could not find uniform buffer.");
		return nullptr;
	}
	return i->second;
}

Shader* ShaderLibrary::GetShaderFromIllumModel(int model)
{
	if (model >= MAX_ILLUM_MODELS || model < 0)
	{
		CC_ERROR("Invalid illumination model or not supported.\n");
		return illumModelShaders_[1];
	}

	return illumModelShaders_[model];
}

void ShaderLibrary::UpdateUniformBufferBindings()
{
	for (const auto& ubo : uniformBuffers_)
	{
		const auto& bindingID = std::distance(uniformBuffers_.begin(), uniformBuffers_.find(ubo.first));
		ubo.second->UpdateShaderBindings(shaders_, bindingID);
	}
}

bool ShaderLibrary::Exists(const std::string& name) const
{
	return shaders_.find(name) != shaders_.end();
}
