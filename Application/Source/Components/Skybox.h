#pragma once
#include "Renderer/Framebuffer.h"
#include <cstdint>
#include <vector>
#include <string>
#include "Renderer/ShaderLibrary.h"
#include "Renderer/Mesh.h"

class Skybox
{
public:
	/// <summary>
	///	Remember the order 
	/// </summary>
	/// <param name="faces">
	/// Order:
	/// +X (right)
	/// -X (left)
	/// +Y (top)
	/// -Y (bottom)
	/// +Z (front)
	/// -Z (back)
	/// </param>
	Skybox(std::vector<std::string> faces);
	virtual ~Skybox();

	void Bind(uint32_t slot = GL_TEXTURE11);
	void Unbind();

	void Render(const glm::mat4& camera_v, const glm::mat4& p);
	void SetRotation(glm::mat4 rotation);

private:
	Framebuffer* framebuffer_;
	uint32_t textureID_;

	glm::mat4 rotationMatrix_;

	Mesh* box_;

	//static ShaderLibrary skyboxShaderLibrary_;
	Shader* skyboxShader_;

	friend class JungleScene;
	friend class BottleThrowScene;
	friend class MainScene;
	friend class BumperCarScene;

	uint32_t vertexArray_, vertexBuffer_;
};

