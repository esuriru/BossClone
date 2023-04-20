#include "Mesh.h"
#include <glad/glad.h>
#include "Utils/Vertex.h"

/******************************************************************************/
/*!
\brief
Default constructor - generate VBO/IBO here

\param meshName - name of mesh
*/
/******************************************************************************/
Mesh::Mesh(const std::string &meshName)
	: name(meshName)
	, indexSize(0)
	, mode(DRAW_MODE::DRAW_TRIANGLES)
{
	glCreateVertexArrays(1, &vertexArray);
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer);
}

Mesh::Mesh(Mesh&& other)
{
	other.vertexArray = 0;
	other.vertexBuffer = 0;
	other.indexBuffer = 0;
}

Mesh& Mesh::operator=(Mesh&& other)
{
	if (this != &other)
	{
		Release();
		std::swap(vertexArray, other.vertexArray);
		std::swap(vertexBuffer, other.vertexBuffer);
		std::swap(indexBuffer, other.indexBuffer);
	}
	return *this;
}

/******************************************************************************/
/*!
\brief
Destructor - delete VBO/IBO here
*/
/******************************************************************************/
Mesh::~Mesh()
{
	//for (auto& material : materials)
	//	if (material)
	//	{
	//		if (material->data)
	//		{
	//			delete material->data;
	//			material->data = nullptr;
	//		}
	//		delete material;
	//		material = nullptr;
	//	}
	CC_TRACE("Mesh destructor called.");
	Release();
}

void Mesh::Release()
{
	//CC_TRACE("Vertex array ID to be deleted: ", vertexArray, ". Vertex buffer ID to be deleted: ", vertexBuffer, ". IndexBuffer ID to be deleted: ", indexBuffer);
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);


	//if (loadedModel)
	//{
	//	for (auto& i : materials)
	//	{
	//		delete i;
	//		i = nullptr;
	//	}
	//	materials.clear();
	//}
}

/******************************************************************************/
/*!
\brief
OpenGL render code
*/
/******************************************************************************/
void Mesh::Render()
{
	glEnableVertexAttribArray(0); // 1st attribute buffer : positions
	glEnableVertexAttribArray(1); // 2nd attribute buffer : colors
	glEnableVertexAttribArray(2); // 2nd attribute buffer : normals 

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4)));
	if (textureID > 0)
	{
		glEnableVertexAttribArray(3); // 2nd attribute buffer : colors
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec3)));
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	if (mode == DRAW_MODE::DRAW_TRIANGLE_STRIP)
		glDrawElements(GL_TRIANGLE_STRIP, indexSize, GL_UNSIGNED_INT, 0);
	else if (mode == DRAW_MODE::DRAW_LINES)
		glDrawElements(GL_LINES, indexSize, GL_UNSIGNED_INT, 0);
	else
		glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	if (textureID > 0)
		glDisableVertexAttribArray(3);
}

void Mesh::Render(UniformBuffer* materialUBO)
{
#if 0
	glEnableVertexAttribArray(0); // 1st attribute buffer : positions
	glEnableVertexAttribArray(1); // 2nd attribute buffer : colors
	glEnableVertexAttribArray(2); // 3nd attribute buffer : normals 
	glEnableVertexAttribArray(3); // 4nd attribute buffer : uvs 

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec3)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	uint32_t offset = 0;
	bool textureEnabled = false;
	bool mapEnabled = true;
	if (materials.size() > 1 && !materials.empty())
		for (const auto& m : materials)
		{
			materialUBO->SetData(m->data, sizeof(MaterialData));
			if (m->textureMaps[Material::DIFFUSE])
			{
				textureEnabled = true;
				m->textureMaps[Material::DIFFUSE]->Bind();
				m->materialShader->SetInt("u_textureEnabled", 1);
				m->materialShader->SetInt("u_tex", 0);
				m->materialShader->SetFloat("u_tilingFactor", m->tilingFactor);

				if (m->materialShader->HasFlag(ShaderFlag::MULTI_TEXTURING))
				{
					for (int i = Material::R; i < Material::NUM_TEXTURE_TYPES; ++i)
					{
						if (m->rgbTextures[i])
						{
							m->rgbTextures[i]->Bind(i + 1);
							m->materialShader->SetInt(Material::GetUniformNameFromTextureType(static_cast<Material::TextureType>(i)), i + 1);
						}
					}
				}

			}

			for (int i = 0; i < Material::MapType::NUM_MAP_TYPES; ++i)
			{
				const auto& map = m->textureMaps[i];
				if (map)
				{
					for (int j = 0; j <= ShaderFlag::SPECULAR_MAPPED; j = BIT(j))
					{
						if (m->materialShader->HasFlag(static_cast<ShaderFlag>(j)))
						{
							map->Bind(j);
							m->materialShader->SetInt(GetUniformNameFromShaderFlag(j), j);
							mapEnabled = true;
						}
					}
				}
			}

			if (mode == DRAW_MODE::DRAW_TRIANGLE_STRIP)
				glDrawElements(GL_TRIANGLE_STRIP, m->size, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned)));
			else if (mode == DRAW_MODE::DRAW_LINES)
				glDrawElements(GL_LINES, m->size, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned)));
			else
				glDrawElements(GL_TRIANGLES, m->size, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned)));

			offset += m->size;

			if (textureEnabled)
			{
				m->textureMaps[Material::DIFFUSE]->Unbind();
				m->materialShader->SetInt("u_textureEnabled", 0);
				m->materialShader->SetFloat("u_tilingFactor", 1.0f);
				textureEnabled = false;
			}

			if (mapEnabled)
			{
				glBindTexture(GL_TEXTURE_2D, 0);
				mapEnabled = false;
			}
		}
	else
	{
		const auto& m = materials.front();
		for (int i = 0; i < Material::MapType::NUM_MAP_TYPES; ++i)
		{
			const auto& map = m->textureMaps[i];
			if (map)
			{
				for (int j = 0; j <= ShaderFlag::SPECULAR_MAPPED; j = BIT(j))
				{
					if (m->materialShader->HasFlag(static_cast<ShaderFlag>(j)))
					{
						map->Bind(j);
						m->materialShader->SetInt(GetUniformNameFromShaderFlag(j), j);
						mapEnabled = true;
					}
				}
			}

		}
		if (m->textureMaps[Material::DIFFUSE])
		{
			textureEnabled = true;
			m->textureMaps[Material::DIFFUSE]->Bind();
			m->materialShader->SetInt("u_textureEnabled", 1);
			m->materialShader->SetInt("u_tex", 0);
			m->materialShader->SetFloat("u_tilingFactor", m->tilingFactor);
			if (m->materialShader->HasFlag(ShaderFlag::MULTI_TEXTURING))
			{
				for (int i = Material::R; i < Material::NUM_TEXTURE_TYPES; ++i)
				{
					if (m->rgbTextures[i])
					{
						m->rgbTextures[i]->Bind(GL_TEXTURE1 + i);
						m->materialShader->SetInt(Material::GetUniformNameFromTextureType(static_cast<Material::TextureType>(i)), i + 1);
					}
				}
			}
		}

		materialUBO->SetData(materials.front()->data, sizeof(MaterialData));
		if (mode == DRAW_MODE::DRAW_TRIANGLE_STRIP)
			glDrawElements(GL_TRIANGLE_STRIP, indexSize, GL_UNSIGNED_INT, 0);
		else if (mode == DRAW_MODE::DRAW_LINES)
			glDrawElements(GL_LINES, indexSize, GL_UNSIGNED_INT, 0);
		else
			glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);

		if (m->textureMaps[Material::DIFFUSE])
		{
			m->textureMaps[Material::DIFFUSE]->Unbind();
			m->materialShader->SetInt("u_textureEnabled", 0);
			m->materialShader->SetFloat("u_tilingFactor", 1.0f);
			textureEnabled = false;
		}

		if (mapEnabled)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			mapEnabled = false;
		}
	}

#if 0
	if (mode == DRAW_MODE::DRAW_TRIANGLE_STRIP)
		glDrawElements(GL_TRIANGLE_STRIP, indexSize, GL_UNSIGNED_INT, 0);
	else if (mode == DRAW_MODE::DRAW_LINES)
		glDrawElements(GL_LINES, indexSize, GL_UNSIGNED_INT, 0);
	else
		glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
#endif

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
#endif
}

void Mesh::Render(unsigned offset, unsigned count)
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(0));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(glm::vec3)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(glm::vec3) + sizeof(glm::vec4)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec3)));

	//glDrawArrays(GL_TRIANGLES, offset, count);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	if (mode == DRAW_MODE::DRAW_LINES)
		glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(offset * sizeof(GLuint)));
	else if (mode == DRAW_MODE::DRAW_TRIANGLE_STRIP)
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(offset * sizeof(GLuint)));
	else
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(offset * sizeof(GLuint)));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

void Mesh::RenderEnvironmentMapped()
{
	glEnableVertexAttribArray(0); // 1st attribute buffer : positions
	glEnableVertexAttribArray(1); // 2nd attribute buffer : normals 

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	if (mode == DRAW_MODE::DRAW_TRIANGLE_STRIP)
		glDrawElements(GL_TRIANGLE_STRIP, indexSize, GL_UNSIGNED_INT, 0);
	else if (mode == DRAW_MODE::DRAW_LINES)
		glDrawElements(GL_LINES, indexSize, GL_UNSIGNED_INT, 0);
	else
		glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}
