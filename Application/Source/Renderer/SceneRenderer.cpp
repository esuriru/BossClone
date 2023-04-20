#include "SceneRenderer.h"

#include <iostream>
#include <algorithm>

#include "Scene/GameObject.h"
#include "Core/Application.h"
#include "Core/Core.h"
#include <glad/glad.h>
#include "MeshBuilder.h"
#include "Shader.h"
#include "Utils/LoadTGA.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#if 0
#	define RENDER_WITH_DEFAULT
#endif
#include "Components/Text.h"

static GLenum ConvertDrawModeToEnum(Mesh::DRAW_MODE mode)
{
	switch (mode)
	{
	case Mesh::DRAW_MODE::DRAW_LINES:
		return GL_LINES;
	case Mesh::DRAW_MODE::DRAW_TRIANGLES:
		return GL_TRIANGLES;
	case Mesh::DRAW_MODE::DRAW_TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	};

	CC_ASSERT(false, "Draw mode not supported.");
}


void SceneRenderer::OnWindowResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	orthoProjection_ = glm::ortho(0.f, static_cast<float>(width),
		0.f, static_cast<float>(height), -10.f, 10.f);
}


void SceneRenderer::Reset()
{
	refSkybox_ = nullptr;
	refCamera_ = nullptr;
	sceneShaderLibrary_ = nullptr;
	lightBlock_ = {};
	auto lightBlock = ShaderLibrary::GetInstance()->GetUniformBuffer("LightBlock");
	if (lightBlock)
		lightBlock->SetData(lightBlock_.data(), sizeof(lightBlock_));
}

void SceneRenderer::BeginScene(Camera* sceneCamera, ShaderLibrary* shaderLibrary, Skybox* skybox, const std::vector<LightData*>& lights, uint32_t numLights)
{
	//glBindVertexArray(vertexArrayID_);
	sceneShaderLibrary_ = shaderLibrary;
	refCamera_ = sceneCamera;
	if (sceneCamera)
	{
		sceneViewMatrix_ = sceneCamera->GetViewMatrix();
		sceneProjectionMatrix_ = sceneCamera->projection;
		cameraPosition_ = refCamera_->GetGameObject()->GetTransform()->GetPosition();
	}
	refSkybox_ = skybox;
	if (!shaderLibrary)
		return;

	// TODO - Move this to where a light is being created so it doesn't get run every frame, just a small optimization.
	numLights = glm::clamp(numLights, static_cast<uint32_t>(0), MAX_SCENE_LIGHTS);

	const auto& lightBlock = sceneShaderLibrary_->GetUniformBuffer("LightBlock");
	if (sceneCamera)
	{
		const auto& viewMatrix = sceneCamera->GetViewMatrix();
		//std::cout << glm::to_string(viewMatrix) << "\n";
		for (int i = 0; i < numLights; ++i)
		{
			const auto& light = lights[i];
			if (light)
			{
				lightBlock_[i] = *light;
				
				// Move to camera space
				lightBlock_[i].centre = viewMatrix * glm::vec4(light->centre.x, light->centre.y, light->centre.z, 1.0f);
				lightBlock_[i].direction = viewMatrix * glm::vec4(light->direction.x, light->direction.y, light->direction.z, 0.0f);
			}
			
		}
	}

	if (lightBlock)
		lightBlock->SetData(lightBlock_.data(), sizeof(lightBlock_));

	numLights_ = numLights;
}

void SceneRenderer::BeginScene(const glm::mat4& sceneView, const glm::mat4& sceneProjection, const glm::vec3& sceneCameraPostiion, ShaderLibrary* shaderLibrary, Skybox* skybox, const std::vector<LightData*>& lights, uint32_t numLights)
{
	//glBindVertexArray(vertexArrayID_);
	sceneShaderLibrary_ = shaderLibrary;
	sceneViewMatrix_ = sceneView;
	sceneProjectionMatrix_ = sceneProjection;
	cameraPosition_ = sceneCameraPostiion;
	refSkybox_ = skybox;

	numLights = glm::clamp(numLights, static_cast<uint32_t>(0), MAX_SCENE_LIGHTS);

	const auto& lightBlock = sceneShaderLibrary_->GetUniformBuffer("LightBlock");
	const auto& viewMatrix = sceneView;
	//std::cout << glm::to_string(viewMatrix) << "\n";
	for (int i = 0; i < numLights; ++i)
	{
		const auto& light = lights[i];
		if (light)
		{
			lightBlock_[i] = *light;
			
			// Move to camera space
			lightBlock_[i].centre = viewMatrix * glm::vec4(light->centre.x, light->centre.y, light->centre.z, 1.0f);
			lightBlock_[i].direction = viewMatrix * glm::vec4(light->direction.x, light->direction.y, light->direction.z, 0.0f);
		}
		
	}

	if (lightBlock)
		lightBlock->SetData(lightBlock_.data(), sizeof(lightBlock_));

	numLights_ = numLights;
}

void SceneRenderer::BeginScene(Camera* sceneCamera, ShaderLibrary* shaderLibrary)
{
	//glBindVertexArray(vertexArrayID_);
	sceneShaderLibrary_ = shaderLibrary;
	if (!sceneCamera)
		return;
	sceneViewMatrix_ = sceneCamera->GetViewMatrix();
	sceneProjectionMatrix_ = sceneCamera->projection;
}

void SceneRenderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneRenderer::SetClearColour(const glm::vec4& colour)
{
	glClearColor(colour.r, colour.g, colour.b, colour.a);
}

void SceneRenderer::EndScene()
{
	Flush();
}

void SceneRenderer::QueueRender(Renderer* r)
{
	if (r->renderOrder > MAX_RENDER_LAYERS)
		std::cout << "Could not render something that is out of the render layers.\n";

	renderLayers_[r->renderOrder].emplace_back(r);
}

void SceneRenderer::RenderMesh(Mesh* mesh, const glm::mat4& model, bool enableLight)
{
	const auto& materialUniformBuffer = sceneShaderLibrary_->GetUniformBuffer("Material");
	uint32_t offset = 0;

	for (const auto& m : mesh->materials)
	{
		if (!m)
			continue;

		auto& shader = m->materialShader;

		if (!shader)
		{
			CC_ERROR("Mesh could not be rendered because its material has no shader.");
			return;
		}
		const auto& MV = sceneViewMatrix_ * model;
		const auto& MVP = sceneProjectionMatrix_ * MV;

		shader->Bind();

		shader->SetMat4("MVP", MVP);

		if (shader->lit)
		{
			shader->SetInt("u_textEnabled", 0);
			shader->SetInt("lightEnabled", true);
			shader->SetInt("numLights", static_cast<int>(numLights_));
			const auto& MVit = glm::transpose(glm::inverse(MV));

			shader->SetMat4("MV", MV);
			shader->SetMat4("MV_inverse_transpose", MVit);
		}

		if (shader->HasFlag(ShaderFlag::NORMAL_MAPPED))
		{
			shader->SetMat4("M", model);
		}

		if (shader->HasFlag(ShaderFlag::ENVIRONMENT_MAPPED))
		{
			shader->SetMat4("model", model);
			shader->SetMat4("view", sceneViewMatrix_);
			shader->SetMat4("projection", sceneProjectionMatrix_);
			refSkybox_->Bind();

			shader->SetInt(GetUniformNameFromShaderFlag(ShaderFlag::ENVIRONMENT_MAPPED), 11);
			shader->SetFloat3("cameraPos", cameraPosition_);
		}

		materialUniformBuffer->SetData(m->data, sizeof(MaterialData));

		if (shader->HasFlag(ShaderFlag::MULTI_TEXTURING))
		{
			if (m->textureMaps[Material::MapType::DIFFUSE])
			{
				shader->SetInt("u_textureEnabled", 1);
				m->textureMaps[Material::MapType::DIFFUSE]->Bind();
				shader->SetInt("u_tex", 0);
				for (int i = Material::R; i < Material::NUM_TEXTURE_TYPES; ++i)
				{
					if (m->rgbTextures[i])
					{
						m->rgbTextures[i]->Bind(i + 1);
						shader->SetInt(Material::GetUniformNameFromTextureType(static_cast<Material::TextureType>(i)), i + 1);
					}
				}
			}

		}
		else
		{
			uint32_t dI = 0, sI = 0, nI = 0, hI = 0;
			for (int i = 0; i < Material::MapType::NUM_MAP_TYPES; ++i)
			{
				//glActiveTexture(GL_TEXTURE0 + i);

				const auto& texture = m->textureMaps[i];
				if (!texture)
					continue;

				std::string index;
				switch (static_cast<Material::MapType>(i))
				{
				case Material::MapType::AMBIENT:
					shader->SetInt("u_ambient_map_enabled", 1);
					index = std::to_string(hI++);
					break;
				case Material::MapType::DIFFUSE:
					shader->SetInt("u_diffuse_map_enabled", 1);
					index = std::to_string(dI++);
					break;
				case Material::MapType::SPECULAR:
					shader->SetInt("u_specular_map_enabled", 1);
					index = std::to_string(sI++);
					break;
				case Material::MapType::NORMAL:
					shader->SetInt("u_normal_map_enabled", 1);
					index = std::to_string(nI++);
					break;
				}

				texture->Bind(i);
				shader->SetInt(Material::GetUniformNameFromMapType(static_cast<Material::MapType>(i)).append(index), i);
			}

		}


		shader->SetFloat("u_tilingFactor", m->tilingFactor);

		DrawIndexed(mesh, m->size, offset);

		//glActiveTexture(GL_TEXTURE0);
		shader->SetInt("u_ambient_map_enabled",  0);
		shader->SetInt("u_diffuse_map_enabled",  0);
		shader->SetInt("u_specular_map_enabled", 0);
		shader->SetInt("u_normal_map_enabled",   0);

		shader->SetInt("u_textureEnabled", 0);
		offset += m->size;

		if (shader->HasFlag(ShaderFlag::ENVIRONMENT_MAPPED))
		{
			refSkybox_->Unbind();
		}
	}
}

void SceneRenderer::RenderMeshOnScreen(Mesh* mesh, const glm::mat4& model)
{
	const auto& materialUniformBuffer = sceneShaderLibrary_->GetUniformBuffer("Material");
	uint32_t offset = 0;

	for (const auto& m : mesh->materials)
	{
		auto& shader = m->materialShader;

		if (!shader)
		{
			std::cout << "Mesh could not be rendered because its material has no shader.\n";
			return;
		}

		glDisable(GL_DEPTH_TEST);
		const auto& MV = model;
		const auto& MVP = orthoProjection_ * MV;

		shader->Bind();

		shader->SetMat4("MVP", MVP);

		if (shader->lit)
		{
			shader->SetInt("u_textEnabled", 0);
			shader->SetInt("lightEnabled", true);
			shader->SetInt("numLights", static_cast<int>(numLights_));
			const auto& MVit = glm::transpose(glm::inverse(MV));

			shader->SetMat4("MV", MV);
			shader->SetMat4("MV_inverse_transpose", MVit);
		}

		if (shader->HasFlag(ShaderFlag::NORMAL_MAPPED))
		{
			shader->SetMat4("model", model);
		}

		if (shader->HasFlag(ShaderFlag::ENVIRONMENT_MAPPED))
		{
			shader->SetMat4("model", model);
			shader->SetMat4("view", sceneViewMatrix_);
			shader->SetMat4("projection", sceneProjectionMatrix_);
			refSkybox_->Bind();

			shader->SetInt(GetUniformNameFromShaderFlag(ShaderFlag::ENVIRONMENT_MAPPED), 11);
			shader->SetFloat3("cameraPos", cameraPosition_);
		}

	
		materialUniformBuffer->SetData(m->data, sizeof(MaterialData));

		uint32_t dI = 0, sI = 0, nI = 0, hI = 0;
		for (int i = 0; i < Material::MapType::NUM_MAP_TYPES; ++i)
		{
			//glActiveTexture(GL_TEXTURE0 + i);

			const auto& texture = m->textureMaps[i];
			if (!texture)
				continue;

			std::string index;
			switch (static_cast<Material::MapType>(i))
			{
			case Material::MapType::AMBIENT:
				shader->SetInt("u_ambient_map_enabled", 1);
				index = std::to_string(hI++);
				break;
			case Material::MapType::DIFFUSE:
				shader->SetInt("u_diffuse_map_enabled", 1);
				index = std::to_string(dI++);
				break;
			case Material::MapType::SPECULAR:
				shader->SetInt("u_specular_map_enabled", 1);
				index = std::to_string(sI++);
				break;
			case Material::MapType::NORMAL:
				shader->SetInt("u_normal_map_enabled", 1);
				index = std::to_string(nI++);
				break;
			}

			texture->Bind(i);
			shader->SetInt(Material::GetUniformNameFromMapType(static_cast<Material::MapType>(i)).append(index), i);

			//if (i == Material::MapType::DIFFUSE) // To change
			//{
			//	shader->SetInt("u_textureEnabled", 1);
			//}
		}
		shader->SetFloat("u_tilingFactor", m->tilingFactor);

		DrawIndexed(mesh, m->size, offset);

		//glActiveTexture(GL_TEXTURE0);
		shader->SetInt("u_ambient_map_enabled",  0);
		shader->SetInt("u_diffuse_map_enabled",  0);
		shader->SetInt("u_specular_map_enabled", 0);
		shader->SetInt("u_normal_map_enabled",   0);

		shader->SetInt("u_textureEnabled", 0);
		offset += m->size;

		if (shader->HasFlag(ShaderFlag::ENVIRONMENT_MAPPED))
		{
			refSkybox_->Unbind();
		}
		glEnable(GL_DEPTH_TEST);

	}
}

void SceneRenderer::RenderMeshBillboarded(Mesh* mesh, const glm::vec3& worldPosition, const glm::vec2& worldScale)
{
	// TODO - Not sure about this.
	if (mesh->materials.empty())
		return;

	auto shader = Shader::BillboardShader;
	shader->Bind();
	uint32_t dI = 0, sI = 0, nI = 0, hI = 0;
	for (int i = 0; i < Material::MapType::NUM_MAP_TYPES; ++i)
	{
		//glActiveTexture(GL_TEXTURE0 + i);

		const auto& texture = mesh->materials.front()->textureMaps[i];
		if (!texture)
			continue;

		std::string index;
		switch (static_cast<Material::MapType>(i))
		{
		case Material::MapType::AMBIENT:
			shader->SetInt("u_ambient_map_enabled", 1);
			index = std::to_string(hI++);
			break;
		case Material::MapType::DIFFUSE:
			shader->SetInt("u_diffuse_map_enabled", 1);
			index = std::to_string(dI++);
			break;
		case Material::MapType::SPECULAR:
			shader->SetInt("u_specular_map_enabled", 1);
			index = std::to_string(sI++);
			break;
		case Material::MapType::NORMAL:
			shader->SetInt("u_normal_map_enabled", 1);
			index = std::to_string(nI++);
			break;
		}

		texture->Bind(i);
		shader->SetInt(Material::GetUniformNameFromMapType(static_cast<Material::MapType>(i)).append(index), i);

		//if (i == Material::MapType::DIFFUSE) // To change
		//{
		//	shader->SetInt("u_textureEnabled", 1);
		//}
	}


	shader->SetFloat("u_tilingFactor", mesh->materials.front()->tilingFactor);
	const auto& materialUniformBuffer = sceneShaderLibrary_->GetUniformBuffer("Material");

	shader->SetMat4("view", sceneViewMatrix_);
	shader->SetMat4("projection", sceneProjectionMatrix_);
	shader->SetFloat3("position", worldPosition);
	shader->SetFloat2("scale", worldScale);

	DrawIndexed(mesh);

	//Shader* previousShader = mesh->materials.front()->materialShader;
	//mesh->materials.front()->materialShader = shader;
	


	//mesh->materials.front()->materialShader = previousShader;


}

void SceneRenderer::RenderText(Text* text)
{
	text->Render(sceneViewMatrix_, sceneProjectionMatrix_);
}

constexpr glm::mat4 identity = glm::mat4(1.0f);
void SceneRenderer::RenderTextOnScreen(Text* text)
{
	//text->Render(_refCamera->GetViewMatrix(), _orthoProjection);
	text->Render(
		identity,
		orthoProjection_);
}

void SceneRenderer::Flush()
{
	// TODO - To change

	// Opaque
	for (auto& i : renderLayers_.front())
		i->Render();

	// The skybox should be in a layer of it's own.
	if (refSkybox_)
		refSkybox_->Render(sceneViewMatrix_, sceneProjectionMatrix_);
	//_refSkybox->Unbind();

	for (int i = 1; i < MAX_RENDER_LAYERS; ++i)
	{
		auto& render_layer = renderLayers_[i];
		for (auto& i : render_layer)
		{
			//if (i->GetGameObject()->tag == "Dialogue Box")
			//	std::cout << "Da one.\n";
			i->Render();
		}
	}

	for (auto& i : renderLayers_)
		i.clear();

}

Mesh* SceneRenderer::GetMesh(GEOMETRY_TYPE t) const
{
	return meshList_[t];
}

const std::array<Mesh*, SceneRenderer::NUM_GEOMETRY> SceneRenderer::GetMeshList(void) const
{
	return meshList_;
}

SceneRenderer::SceneRenderer()
	: sceneShaderLibrary_()
	//, orthoProjection_(glm::ortho(0.f, static_cast<float>(SCR_WIDTH), 0.f, static_cast<float>(SCR_HEIGHT), -10.f, 10.f))
	, canvasView_(
		glm::lookAt(
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		)
	)
{
#pragma region Mesh_Initialization
	for (auto& i : meshList_)
		i = nullptr;

	meshList_[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList_[GEO_BALL] = MeshBuilder::GenerateSphere("Sphere", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, 32, 1.f);
	meshList_[GEO_BALL2] = MeshBuilder::GenerateSphere("Sphere", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, 32, 1.f);
	//_meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),  1.f);
	meshList_[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),  0.5f);
	meshList_[GEO_TRIANGLE] = MeshBuilder::GenerateTriangle("Triangle", glm::vec4(1.0f, 0, 0, 1.0f));
	meshList_[GEO_QUAD] = MeshBuilder::GenerateQuad("Quad", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshList_[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList_[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

#pragma endregion Mesh_Initialization

#pragma region OpenGL_Initialization
	// Set background color to dark blue

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	//Enable back face culling
	glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	orthoProjection_ = glm::ortho(0.f, static_cast<float>(Application::GetInstance()->GetWindowWidth()),
		0.f, static_cast<float>(Application::GetInstance()->GetWindowHeight()), -10.f, 10.f);

	// Generate a DefaultShader VAO for now
	Shader::DefaultShader->Bind();

#pragma endregion OpenGL_Initialization
}

SceneRenderer::~SceneRenderer()
{
	for (auto& i : meshList_)
		if (i)
			delete i;
}

void SceneRenderer::DrawIndexed(Mesh* mesh, uint32_t indexCount)
{
	glBindVertexArray(mesh->vertexArray);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	uint32_t count = indexCount ? indexCount : mesh->indexSize;
	glDrawElements(ConvertDrawModeToEnum(mesh->mode), static_cast<GLint>(count), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void SceneRenderer::DrawIndexed(Mesh* mesh, uint32_t indexCount, uint32_t offset)
{
	glBindVertexArray(mesh->vertexArray);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	uint32_t count = indexCount ? indexCount : mesh->indexSize;
	glDrawElements(ConvertDrawModeToEnum(mesh->mode), static_cast<GLint>(count), GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
	glBindVertexArray(0);
}
