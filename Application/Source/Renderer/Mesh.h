#ifndef MESH_H
#define MESH_H

#include <string>

#include "Material.h"
#include "UniformBuffer.h"

/******************************************************************************/
/*!
		Class Mesh:
\brief	To store VBO (vertex & color buffer) and IBO (index buffer)
*/
/******************************************************************************/
class Mesh
{
public:
	enum class DRAW_MODE
	{
		DRAW_TRIANGLES, //DefaultShader mode
		DRAW_TRIANGLE_STRIP,
		DRAW_LINES,
		DRAW_MODE_LAST,
	};
	Mesh(const std::string &meshName);
	Mesh(const Mesh& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;

	Mesh(Mesh&& other);
	Mesh& operator=(Mesh&& other);
	~Mesh();

	void Release();
	void Render();
	void Render(UniformBuffer* materialUBO);
	void Render(unsigned offset, unsigned count);
	void RenderEnvironmentMapped();

	const std::string name;
	DRAW_MODE mode;
	unsigned vertexArray;
	unsigned vertexBuffer;
	unsigned indexBuffer;
	unsigned indexSize;

	//Material* material;
	std::vector<Material*> materials;

	unsigned textureID;

	bool loadedModel = false;
	std::array<GLuint, 6> attributeBuffers{};



};

#endif