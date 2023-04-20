#pragma once
#include <string>
#include <glad/glad.h>

#include "Shader.h"

class UniformBuffer
{
public:
	~UniformBuffer();

	void Bind();
	void Unbind();

	void SetData(const void* data, uint32_t size = 0);

	bool operator==(const UniformBuffer& rhs);

private:
	UniformBuffer() = default;

	friend class ShaderLibrary;

	// Safer option
	static UniformBuffer* CreateUniformBuffer(const std::string& name, std::unordered_map<std::string, Shader*>& shaders, GLuint bindingID);

	UniformBuffer(const std::string& name, const std::vector<Shader*>& shaders, GLuint bindingID);
	UniformBuffer(const std::string& name, std::unordered_map<std::string, Shader*>& shaders, GLuint bindingID);

	void UpdateShaderBindings(std::unordered_map<std::string, Shader*>& shaders, GLuint bindingID);

	GLuint bufferID_;
	GLuint blockID_;
	GLint size_;
	std::string name_;

};
