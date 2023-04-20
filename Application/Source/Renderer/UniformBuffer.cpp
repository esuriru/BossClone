#include "UniformBuffer.h"
#include <iostream>

UniformBuffer::UniformBuffer(const std::string& name, std::unordered_map<std::string, Shader*>& shaders, GLuint bindingID)
{
	bool created = false;
	for (const auto& shader_pair : shaders)
	{
		if (created)
			break;

		const auto& shader_id = shader_pair.second->rendererID_;
		const auto& blockID = glGetUniformBlockIndex(shader_id, name.data());
		// If we cannot find the uniform block in the shader, continue.
		if (blockID == GL_INVALID_INDEX)
			continue;

		blockID_ = blockID;
		glUniformBlockBinding(shader_id, blockID, bindingID);
		glGetActiveUniformBlockiv(shader_id, blockID, GL_UNIFORM_BLOCK_DATA_SIZE, &size_);

		glGenBuffers(1, &bufferID_);
		glBindBuffer(GL_UNIFORM_BUFFER, bufferID_);
		glBufferData(GL_UNIFORM_BUFFER, size_, NULL, GL_STATIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingID, bufferID_, 0, size_);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

		created = true;
	}

	if (!created)
		std::cout << "Could not find Uniform Buffer of name '" << name << "'\n";
}

void UniformBuffer::UpdateShaderBindings(std::unordered_map<std::string, Shader*>& shaders, GLuint bindingID)
{
	for (auto& shader_pair : shaders)
	{
		const auto& shader_id = shader_pair.second->rendererID_;
		const auto& block_id = glGetUniformBlockIndex(shader_id, name_.data());
		if (blockID_ == block_id)
		{
			glUniformBlockBinding(shader_id, blockID_, bindingID);
			if (glGetError() == GL_INVALID_VALUE)
			{
				continue;
			}
			if (name_ == "LightBlock" || name_ == "Light")
			{
				shader_pair.second->lit = true;
			}
		}
	}

}

UniformBuffer::UniformBuffer(const std::string& name, const std::vector<Shader*>& shaders, GLuint bindingID)
{
	bool created = false;
	for (const auto& shader : shaders)
	{
		if (created)
			break;

		const auto& shader_id = shader->rendererID_;
		const auto& blockID = glGetUniformBlockIndex(shader_id, name.data());
		// If we cannot find the uniform block in the shader, continue.
		if (blockID == GL_INVALID_INDEX)
			continue;

		blockID_ = blockID;
		glUniformBlockBinding(shader_id, blockID, bindingID);
		glGetActiveUniformBlockiv(shader_id, blockID, GL_UNIFORM_BLOCK_DATA_SIZE, &size_);

		glGenBuffers(1, &bufferID_);
		glBindBuffer(GL_UNIFORM_BUFFER, bufferID_);
		glBufferData(GL_UNIFORM_BUFFER, size_, NULL, GL_STATIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingID, bufferID_, 0, size_);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

		created = true;
	}

	if (!created)
		std::cout << "Could not find Uniform Buffer of name '" << name << "'\n";
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &bufferID_);
}

void UniformBuffer::Bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, bufferID_);
}

void UniformBuffer::Unbind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::SetData(const void* data, uint32_t size)
{
	const auto& data_size = size == 0 ? size_ : size;
	glBindBuffer(GL_UNIFORM_BUFFER, bufferID_);
	glBufferData(GL_UNIFORM_BUFFER, data_size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

bool UniformBuffer::operator==(const UniformBuffer& rhs)
{
	return this->blockID_ == rhs.blockID_;
}

UniformBuffer* UniformBuffer::CreateUniformBuffer(const std::string& name,
                                                  std::unordered_map<std::string, Shader*>& shaders, GLuint bindingID)
{
	if (shaders.empty())
	{
		std::cout << "There are no shaders to find a uniform buffer from.";
		return nullptr;
	}

	UniformBuffer* temp = nullptr;

	bool created = false;

	for (auto& shader_pair : shaders)
	{
		if (created)
			break;

		const auto& shader_id = shader_pair.second->rendererID_;
		const auto& block_id = glGetUniformBlockIndex(shader_id, name.data());
		// If we cannot find the uniform block in the shader, continue.
		if (block_id == GL_INVALID_INDEX)
			continue;

		// When it is found, create the uniform buffer.

		temp = new UniformBuffer();

		temp->blockID_ = block_id;
		glUniformBlockBinding(shader_id, block_id, bindingID);
		glGetActiveUniformBlockiv(shader_id, block_id, GL_UNIFORM_BLOCK_DATA_SIZE, &temp->size_);

		glGenBuffers(1, &temp->bufferID_);
		glBindBuffer(GL_UNIFORM_BUFFER, temp->bufferID_);
		glBufferData(GL_UNIFORM_BUFFER, temp->size_, nullptr, GL_STREAM_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingID, temp->bufferID_, 0, temp->size_);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//if (name == "LightBlock" || name == "Light")
		//{
		//	shader_pair.second->lit = true;
		//}

		created = true;
	}


	if (!created)
	{
		std::cout << "Could not find Uniform Buffer of name '" << name << "'\n";
		return nullptr;
	}

	temp->name_ = name;

	// Now update all of the existing shaders
	for (auto& shader_pair : shaders)
	{
		const auto& shader_id = shader_pair.second->rendererID_;
		const auto& block_id = glGetUniformBlockIndex(shader_id, name.data());
		if (temp->blockID_ == block_id)
		{
			glUniformBlockBinding(shader_id, temp->blockID_, bindingID);
			if (glGetError() == GL_INVALID_VALUE)
			{
				continue;
			}
			if (name == "LightBlock" || name == "Light")
			{
				shader_pair.second->lit = true;
			}
		}
	}

	return temp;
}

