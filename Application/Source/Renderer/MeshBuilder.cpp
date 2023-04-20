#include "MeshBuilder.h"
#include "Core/Core.h"
#include <algorithm>
#include "ShaderLibrary.h"
#include "stb_image.h"
#include "Utils/LoadOBJ.h"
#include <glad/glad.h>
#include <vector>
#include <string>
#include <fstream>
#include "Utils/Vertex.h"
#include "MyMath.h"
#include <iostream>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
/******************************************************************************/
/*!
\brief
Generate the vertices of a reference Axes; Use red for x-axis, green for y-axis, blue for z-axis
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - x-axis should start at -lengthX / 2 and end at lengthX / 2
\param lengthY - y-axis should start at -lengthY / 2 and end at lengthY / 2
\param lengthZ - z-axis should start at -lengthZ / 2 and end at lengthZ / 2

\return Pointer to mesh storing VBO/IBO of reference axes
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	//x-axis
	v.pos = {-lengthX, 0, 0};	v.color = {1, 0, 0, 1};	vertex_buffer_data.push_back(v);
	v.pos = {lengthX, 0, 0};	v.color = {1, 0, 0, 1};	vertex_buffer_data.push_back(v);
	//y-axis
	v.pos = {0, -lengthY, 0};	v.color = {0, 1, 0, 1};	vertex_buffer_data.push_back(v);
	v.pos = {0, lengthY, 0};	v.color = {0, 1, 0, 1};	vertex_buffer_data.push_back(v);
	//z-axis
	v.pos = {0, 0, -lengthZ};	v.color = {0, 0, 1, 1};	vertex_buffer_data.push_back(v);
	v.pos = {0, 0, lengthZ};	v.color = {0, 0, 1, 1};	vertex_buffer_data.push_back(v);

	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(5);

	Mesh *mesh = new Mesh(meshName);
	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_LINES;

	return mesh;
}

/******************************************************************************/
/*!
\brief
Generate the vertices of a quad; Use random color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - width of quad
\param lengthY - height of quad

\return Pointer to mesh storing VBO/IBO of quad
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateQuad(const std::string &meshName, glm::vec4 color, float length)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	// Add the vertices here
	v.pos = { 0.5f * length, -0.5f * length, 0.f };	v.texCoord = { 1, 0 }; vertex_buffer_data.push_back(v); //v3
	v.pos = { 0.5f * length, 0.5f * length, 0.f };	v.texCoord = { 1, 1 };  vertex_buffer_data.push_back(v); //v0
	v.pos = { -0.5f * length, 0.5f * length, 0.f };	v.texCoord = { 0, 1 }; vertex_buffer_data.push_back(v); //v1
	v.pos = { -0.5f * length, -0.5f * length, 0.f };	v.texCoord = { 0, 0 }; vertex_buffer_data.push_back(v); //v2
	for (auto& v : vertex_buffer_data)
		v.normal = { 0, 0 , 1 };

	//tri1
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	//tri2
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);

	// Create the new mesh
	Mesh* mesh = new Mesh(meshName);

	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;

	return mesh;
}

/******************************************************************************/
/*!
\brief
Generate the vertices of a cube; Use random color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - width of cube
\param lengthY - height of cube
\param lengthZ - depth of cube

\return Pointer to mesh storing VBO/IBO of cube
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateCube(const std::string& meshName, glm::vec4 color, float length)
{
#if 0
	Vertex v;
	v.color = color;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	v.pos = {1.f * length, 1.f * length, 1.f * length};  v.normal = glm::vec3(0, 0, 1);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, 1.f * length, 1.f * length};  v.normal = glm::vec3(0, 0, 1);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, -1.f * length, 1.f * length};  v.normal = glm::vec3(0, 0, 1);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, 1.f * length, 1.f * length};  v.normal = glm::vec3(0, 0, 1);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, -1.f * length, 1.f * length};  v.normal = glm::vec3(0, 0, 1);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, -1.f * length, 1.f * length};  v.normal = glm::vec3(0, 0, 1);  vertex_buffer_data.push_back(v);

	v.pos = {1.f * length, 1.f * length, -1.f * length};  v.normal = glm::vec3(1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, 1.f * length, 1.f * length};  v.normal = glm::vec3(1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, -1.f * length, 1.f * length};  v.normal = glm::vec3(1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, 1.f * length, -1.f * length};  v.normal = glm::vec3(1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, -1.f * length, 1.f * length};  v.normal = glm::vec3(1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, -1.f * length, -1.f * length};  v.normal = glm::vec3(1, 0, 0);  vertex_buffer_data.push_back(v);

	v.pos = {1.f * length, 1.f * length, -1.f * length};  v.normal = glm::vec3(0, 1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, 1.f * length, -1.f * length};  v.normal = glm::vec3(0, 1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, 1.f * length, 1.f * length};  v.normal = glm::vec3(0, 1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, 1.f * length, -1.f * length};  v.normal = glm::vec3(0, 1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, 1.f * length, 1.f * length};  v.normal = glm::vec3(0, 1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, 1.f * length, 1.f * length};  v.normal = glm::vec3(0, 1, 0);  vertex_buffer_data.push_back(v);

	v.pos = {-1.f * length, 1.f * length, -1.f * length};  v.normal = glm::vec3(0, 0, -1);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, 1.f * length, -1.f * length};  v.normal = glm::vec3(0, 0, -1);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, -1.f * length, -1.f * length};  v.normal = glm::vec3(0, 0, -1);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, 1.f * length, -1.f * length};  v.normal = glm::vec3(0, 0, -1);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, -1.f * length, -1.f * length};  v.normal = glm::vec3(0, 0, -1);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, -1.f * length, -1.f * length};  v.normal = glm::vec3(0, 0, -1);  vertex_buffer_data.push_back(v);

	v.pos = {-1.f * length, 1.f * length, 1.f * length};  v.normal = glm::vec3(-1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, 1.f * length, -1.f * length};  v.normal = glm::vec3(-1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, -1.f * length, -1.f * length};  v.normal = glm::vec3(-1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, 1.f * length, 1.f * length};  v.normal = glm::vec3(-1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, -1.f * length, -1.f * length};  v.normal = glm::vec3(-1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, -1.f * length, 1.f * length};  v.normal = glm::vec3(-1, 0, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, -1.f * length, -1.f * length};  v.normal = glm::vec3(0, -1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, -1.f * length, -1.f * length};  v.normal = glm::vec3(0, -1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, -1.f * length, 1.f * length};  v.normal = glm::vec3(0, -1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, -1.f * length, -1.f * length};  v.normal = glm::vec3(0, -1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {1.f * length, -1.f * length, 1.f * length};  v.normal = glm::vec3(0, -1, 0);  vertex_buffer_data.push_back(v);
	v.pos = {-1.f * length, -1.f * length, 1.f * length};  v.normal = glm::vec3(0, -1, 0);  vertex_buffer_data.push_back(v);

	for (unsigned i = 0; i < 36; ++i)
	{
		index_buffer_data.push_back(i);
	}

	// Crate Mesh code here
	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
		&vertex_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
		&index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;


	return mesh;
#endif
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	// Create 24 vertices with each normal.
	index_buffer_data = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right

		17, 21, 22,
		22, 18, 17,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		20, 16, 19,
		19, 23, 20,
		// bottom
		12, 13, 9,
		9, 8, 12,
		// top
		11, 10, 14,
		14, 15, 11,
	};

	std::vector<glm::vec3> normals = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),

		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),

		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),

		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(-1, 0, 0),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(-1, 0, 0)
	};

	std::vector<float> vertices = {
		// front
		-length, -length,  length,
		 length, -length,  length,
		 length,  length,  length,
		-length,  length,  length,
		// back
		-length, -length, -length,
		 length, -length, -length,
		 length,  length, -length,
		-length,  length, -length,
		// front
		-length, -length,  length,
		 length, -length,  length,
		 length,  length,  length,
		-length,  length,  length,
		// back
		-length, -length, -length,
		 length, -length, -length,
		 length,  length, -length,
		-length,  length, -length,
		// front
		-length, -length,  length,
		 length, -length,  length,
		 length,  length,  length,
		-length,  length,  length,
		// back
		-length, -length, -length,
		 length, -length, -length,
		 length,  length, -length,
		-length,  length, -length
	};

	std::vector<glm::vec2> textureCoordinates = {
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },

		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },

		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },

		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },

		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },

		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
	};

	for (int i = 0; i < vertices.size() / 3; i++)
	{
		vertex_buffer_data.push_back(Vertex());
		vertex_buffer_data.back().normal = normals[i];
		vertex_buffer_data.back().pos = { vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2] };
		vertex_buffer_data.back().color = color;
		vertex_buffer_data.back().texCoord = textureCoordinates[i];
	}



	// Create the new mesh
	Mesh* mesh = new Mesh(meshName);
	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);
	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;

	return mesh;
#if 0

	Vertex v;
	std::vector<Vertex> vertex_buffer_data;

	v.pos = {-0.5f * length, -0.5f * length, -0.5f * length};
	v.color = color;
	vertex_buffer_data.push_back(v);
	v.pos = {0.5f * length, -0.5f * length, -0.5f * length};
	v.color = color;
	vertex_buffer_data.push_back(v);
	v.pos = {0.5f * length, 0.5f * length, -0.5f * length};
	v.color = color;
	vertex_buffer_data.push_back(v);
	v.pos = {-0.5f * length, 0.5f * length, -0.5f * length};
	v.color = color;
	vertex_buffer_data.push_back(v);
	v.pos = {-0.5f * length, -0.5f * length, 0.5f * length};
	v.color = color;
	vertex_buffer_data.push_back(v);
	v.pos = {0.5f * length, -0.5f * length, 0.5f * length};
	v.color = color;
	vertex_buffer_data.push_back(v);
	v.pos = {0.5f * length, 0.5f * length, 0.5f * length};
	v.color = color;
	vertex_buffer_data.push_back(v);
	v.pos = {-0.5f * length, 0.5f * length, 0.5f * length};
	v.color = color;
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;
	index_buffer_data.push_back(7);
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(6);
	index_buffer_data.push_back(5);
	index_buffer_data.push_back(6);
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(6);
	index_buffer_data.push_back(5);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(5);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(7);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(6);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(7);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(7);
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(7);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(5);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(5);

	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = 36;
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;

	return mesh;
#endif
}

Mesh* MeshBuilder::GenerateRing(const std::string& meshName, glm::vec4 color, unsigned numSlice, float outerR, float innerR)
{
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	Vertex v;

	float degreePerSlice = 360.f / numSlice;
	//float radianPerSlice = Math::DegreeToRadian(360.f) / numSlice;
	for (unsigned slice = 0; slice < numSlice + 1; ++slice)
	{
		float theta = slice * degreePerSlice;
		v.color = color;
		v.normal = glm::vec3 (0, 1, 0);
		v.pos = {outerR * cos(Math::DegreeToRadian(theta)), 0, outerR * sin(Math::DegreeToRadian(theta))};
		vertex_buffer_data.push_back(v);

		v.color = color;
		v.normal = glm::vec3(0, 1, 0);
		v.pos = {innerR * cos(Math::DegreeToRadian(theta)), 0, innerR * sin(Math::DegreeToRadian(theta))};
		vertex_buffer_data.push_back(v);
	}
	for (unsigned slice = 0; slice < numSlice + 1; ++slice)
	{
		index_buffer_data.push_back(2 * slice + 0);
		index_buffer_data.push_back(2 * slice + 1);
		//index_buffer_data.push_back(2 * slice + 3);
		//index_buffer_data.push_back(2 * slice + 4);
		//index_buffer_data.push_back(2 * slice + 3);
		//index_buffer_data.push_back(2 * slice + 2);
	}

	Mesh* mesh = new Mesh(meshName);

	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLE_STRIP;

	mesh->indexSize = index_buffer_data.size();

	return mesh;
}

float sphereX(float phi, float theta)
{
	return cos(Math::DegreeToRadian(phi)) * cos(Math::DegreeToRadian(theta));
}
float sphereY(float phi, float theta)
{
	return sin(Math::DegreeToRadian(phi));
}
float sphereZ(float phi, float theta)
{
	return cos(Math::DegreeToRadian(phi)) * sin(Math::DegreeToRadian(theta));
}

Mesh* MeshBuilder::GenerateSphere(const std::string& meshName, glm::vec4 color, unsigned numStack, unsigned numSlice, float radius)
{
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	float degreePerStack = 180.f / numStack;
	float degreePerSlice = 360.f / numSlice;
	//float radianPerSlice = Math::DegreeToRadian(360.f) / numSlice;

	for (unsigned stack = 0; stack < numStack + 1; ++stack) //stack //replace with 180 for sphere
	{
		float phi = -90.f + stack * degreePerStack;
		for (unsigned slice = 0; slice < numSlice + 1; ++slice) //slice
		{
			float theta = slice * degreePerSlice;
			Vertex v;
			v.pos = {radius * sphereX(phi, theta), radius * sphereY(phi, theta), radius * sphereZ(phi, theta)};
			v.color = color;
			v.normal = glm::vec3(sphereX(phi, theta), sphereY(phi, theta), sphereZ(phi, theta));

			float xTex = 1.f / numSlice;
			float yTex = 1.f / numStack;

			v.texCoord = glm::vec2(1.f - xTex * slice, 1.f - yTex * stack);

			vertex_buffer_data.push_back(v);
		}
	}
	for (unsigned stack = 0; stack < numStack; ++stack)
	{
		for (unsigned slice = 0; slice < numSlice + 1; ++slice)
		{
			index_buffer_data.push_back((numSlice + 1) * stack + slice + 0);
			index_buffer_data.push_back((numSlice + 1) * (stack + 1) + slice + 0);
			//index_buffer_data.push_back((numSlice + 1) * stack + slice + 1);
			//index_buffer_data.push_back((numSlice + 1) * (stack + 1) + slice + 1);
			//index_buffer_data.push_back((numSlice + 1) * stack + slice + 1);
			//index_buffer_data.push_back((numSlice + 1) * (stack + 1) + slice + 0);
		}
	}

	Mesh* mesh = new Mesh(meshName);

	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLE_STRIP;

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mesh->indexSize = index_buffer_data.size();

	//glBindVertexArray(0);

	return mesh;
}

Mesh* MeshBuilder::GenerateCone(const std::string& meshName, glm::vec4 color, unsigned numSlice, float radius, float height)
{
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	Vertex v;
	float degreePerSlice = 360.f / numSlice;

	for (unsigned slice = 0; slice < numSlice + 1; ++slice) //slice
	{
		float theta = slice * degreePerSlice;
		glm::vec3 normal(height * cos(Math::DegreeToRadian(theta)), radius, height * sin(Math::DegreeToRadian(theta)));
		normal = glm::normalize(normal);

		v.pos = {radius * cos(Math::DegreeToRadian(theta)), 0, radius * sin(Math::DegreeToRadian(theta))};
		v.color = color;
		v.normal = normal;
		vertex_buffer_data.push_back(v);

		v.pos = {0, height, 0};
		v.color = color;
		v.normal = normal;
		vertex_buffer_data.push_back(v);
	}
	for (unsigned slice = 0; slice < numSlice + 1; ++slice)
	{
		index_buffer_data.push_back(slice * 2 + 0);
		index_buffer_data.push_back(slice * 2 + 1);
	}

	Mesh* mesh = new Mesh(meshName);

	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLE_STRIP;

	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();

	return mesh;
}

Mesh* MeshBuilder::GenerateTriangle(const std::string& meshName, glm::vec4 color, float length)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	const auto& half_length = length * 0.5f;
	v.pos = {-half_length, -half_length, 0};
	v.color = color;
	vertex_buffer_data.push_back(v);
	v.pos = {half_length, -half_length, 0};
	v.color = color;
	vertex_buffer_data.push_back(v);
	v.pos = {0, half_length, 0};
	v.color = color;
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data = {0, 1, 2};

	Mesh* mesh = new Mesh(meshName);

	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;
	mesh->indexSize = index_buffer_data.size();

	return mesh;
	//vertex_buffer_data = {
	//	-half_length, -half_length, 0.f,
	//	half_length, -half_length, 0.f,
	//	0.f, half_length, 0.f
	//};
	
}


Mesh* MeshBuilder::GenerateText(const std::string& meshName, unsigned numRow, unsigned numCol)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	float width = 1.f / numCol;
	float height = 1.f / numRow;
	int offset = 0;
	for (unsigned i = 0; i < numRow; ++i)
	{
		for (unsigned j = 0; j < numCol; ++j)
		{
			float u1 = j * width;
			float v1 = 1.f - height - i * height;
			v.pos = {-0.5f, -0.5f, 0};
			v.texCoord = {u1, v1};
			vertex_buffer_data.push_back(v);

			v.pos = {0.5f, -0.5f, 0};
			v.texCoord = {u1 + width, v1};
			vertex_buffer_data.push_back(v);

			v.pos = {0.5f, 0.5f, 0};
			v.texCoord = {u1 + width, v1 + height};
			vertex_buffer_data.push_back(v);

			v.pos = {-0.5f, 0.5f, 0};
			v.texCoord = {u1, v1 + height};
			vertex_buffer_data.push_back(v);

			index_buffer_data.push_back(offset + 0);
			index_buffer_data.push_back(offset + 1);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 0);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 3);
			offset += 4;
		}
	}

	Mesh* mesh = new Mesh(meshName);

	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateOBJ(const std::string& meshName, const std::string& file_path)
{
	// Read vertices, texcoords & normals from OBJ
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	bool success = LoadOBJ(file_path.c_str(), vertices, uvs, normals);
	if(!success)
		return nullptr;
	// Index the vertices, texcoords & normals properly
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	IndexVBO(vertices, uvs, normals, index_buffer_data, vertex_buffer_data);
	// Create the mesh as usual when you do other geometries
	// Use triangle list and remember to set index size
	Mesh* mesh = new Mesh(meshName);
	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateOBJMTL(const std::string& meshName, const std::string& file_path, const std::string& mtl_path)
{
	// Read vertices, texcoords & normals from OBJ
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	std::vector<Material*> mats;

	bool success = LoadOBJMTL(file_path.data(), mtl_path.data(), vertices, uvs, normals, mats);
	if (!success)
		return nullptr;

	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	IndexVBO(vertices, uvs, normals, index_buffer_data, vertex_buffer_data);

	Mesh* mesh = new Mesh(meshName);


	std::swap(mats, mesh->materials);

	//for (auto& m : mesh->materials)
	//	m->materialShader = Shader::DefaultShader;

	
	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;
	mesh->loadedModel = true;

	return mesh;
}

Mesh* MeshBuilder::GenerateMappedOBJ(const std::string& meshName, const std::string& file_path, const std::string& mtl_path)
{
	return nullptr;
}

Mesh* MeshBuilder::GenerateMappedOBJ(const std::string& meshName, const std::string& obj_path)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	std::vector<Material*> mats;

	bool success = LoadMappedOBJ(obj_path.data(), vertices, uvs, normals, tangents, bitangents, mats);
	if (!success)
		return nullptr;

	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	IndexVBO_TBN(vertices, uvs, normals, tangents, bitangents, index_buffer_data, vertex_buffer_data);

	Mesh* mesh = new Mesh(meshName);

	std::swap(mats, mesh->materials);

	for (auto& m : mesh->materials)
		m->materialShader = Shader::MappedShader;

	
	FillBuffers(mesh, vertex_buffer_data, index_buffer_data);
	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::Generate(const std::string& meshName, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	Mesh* mesh = new Mesh(meshName);

	FillBuffers(mesh, vertices, indices);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mesh->indexSize = indices.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;

	return mesh;
}

#if 0
constexpr int MAX_HEIGHT = 40;
constexpr int SIZE = 800;
constexpr int MAX_PIXEL_COLOUR = 256 * 256 * 256;
constexpr float HALF_MAX_PIXEL_COLOUR = MAX_PIXEL_COLOUR * 0.5f;
const float INVERSE_HALF_MAX_PIXEL_COLOUR = 1.f / HALF_MAX_PIXEL_COLOUR;


float GetHeight(int x, int z, int width, int height, uint32_t channelCount, unsigned char* data)
{
	if (x < 0 || x >= height || z < 0 || z >= height)
		return 0;

	unsigned bytePerPixel = channelCount;
	unsigned char* pixelOffset = data + (x + width * z) * bytePerPixel;
	unsigned char r = pixelOffset[0];
	unsigned char g = pixelOffset[1];
	unsigned char b = pixelOffset[2];
	unsigned char a = channelCount >= 4 ? pixelOffset[3] : 0xff;

	float vheight = static_cast<float>(r) * static_cast<float>(g) * static_cast<float>(b);
	vheight += HALF_MAX_PIXEL_COLOUR;
	vheight *= INVERSE_HALF_MAX_PIXEL_COLOUR;
	vheight *= MAX_HEIGHT;
	return vheight;
}

glm::vec3 CalculateNormal(int x, int z, int width, int height, uint32_t channelCount, unsigned char* data)
{
	float hL = GetHeight(x - 1, z, width, height, channelCount, data);
	float hR = GetHeight(x + 1, z, width, height, channelCount, data);
	float hD = GetHeight(x, z - 1, width, height, channelCount, data);
	float hU = GetHeight(x, z + 1, width, height, channelCount, data);

	glm::vec3 normal = { hL - hR, 2.f, hD - hU };
	return glm::normalize(normal);
}

Mesh* MeshBuilder::GenerateTerrain(const std::string& meshName, const std::string& height_map, const std::string& texture_path)
{
	// Create buffers
	int width, height, channels;

	// Flip the image
	stbi_set_flip_vertically_on_load(1);
	auto data = stbi_load(height_map.data(), &width, &height, &channels, 0);

	if (!data)
	{
		std::cout << "Failed to load image!\n";
		return nullptr;
	}
	
	const int vert_count = height;
	const int count = vert_count * vert_count;
	
	std::vector<Vertex> vertex_buffer_data;
	//std::vector<uint32_t> indices;
	std::vector<uint32_t> indices;
		
	int vertPointer = 0;
	for (int i = 0; i < vert_count; ++i)
	{
		for (int j = 0; j < vert_count; ++j)
		{
			vertex_buffer_data.push_back(Vertex());
			vertex_buffer_data.back().pos = {static_cast<float>(j) / (static_cast<float>(vert_count) - 1) * SIZE, GetHeight(j, i, width, height, channels, data), static_cast<float>(i) / (static_cast<float>(vert_count) - 1) * SIZE};
			vertex_buffer_data.back().normal = CalculateNormal(j, i, width, height, channels, data);
			vertex_buffer_data.back().texCoord = { static_cast<float>(j) / (static_cast<float>(vert_count) - 1),static_cast<float>(i) / (static_cast<float>(vert_count) - 1) };
			//++vertPointer;
		}
	}

	int pointer = 0;
	for (int gz = 0; gz < vert_count - 1; ++gz)
	{
		for (int gx = 0; gx < vert_count - 1; ++gx)
		{
			int topLeft = (gz * vert_count) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * vert_count) + gx;
			int bottomRight = bottomLeft + 1;

			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}

	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

	mesh->indexSize = indices.size();
	mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;

	mesh->materials.push_back(new Material());
	mesh->materials.back()->materialShader = ShaderLibrary::GetShaderFromIllumModel(1);
	mesh->materials.back()->textureMaps[Material::DIFFUSE] = Texture::LoadTexture(texture_path.data());


	return mesh;
}
#endif


struct MeshData
{
	uint32_t materialIndex;
	uint32_t baseIndex;
	uint32_t baseVertex;
	uint32_t numIndices;
};

Mesh* MeshBuilder::LoadStaticMesh(const std::string& name, const std::string& file_path)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(file_path, CC_ASSIMP_LOAD_FLAGS);
	
	if (scene)
	{
		Mesh* mesh = new Mesh(name);
		glGenVertexArrays(1, &mesh->vertexArray);
		glBindVertexArray(mesh->vertexArray);

		// Maybe make this a diff class instead of just a mesh?
		//mesh->assimpLoaded = true;


		glGenBuffers(mesh->attributeBuffers.size(), mesh->attributeBuffers.data());

		// Data to be retrieved
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> tex_coords;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<uint32_t> indices;

		std::vector<Texture*> textures;

		std::vector<MeshData> mesh_data;
		mesh_data.resize(scene->mNumMeshes);
		textures.resize(scene->mNumMaterials);

		uint32_t vertex_count = 0, indice_count = 0;

		positions.reserve(vertex_count);
		normals.reserve(vertex_count);
		tex_coords.reserve(vertex_count);
		tangents.reserve(vertex_count);
		bitangents.reserve(vertex_count);
		indices.reserve(indice_count);


		// Count vertices and indices.
		for (uint32_t i = 0; i < mesh_data.size(); ++i)
		{
			mesh_data[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
			mesh_data[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
			mesh_data[i].baseVertex = vertex_count;
			mesh_data[i].baseIndex = indice_count;

			vertex_count += scene->mMeshes[i]->mNumVertices;
			vertex_count += mesh_data[i].numIndices;
		}

		const aiVector3D zero_vector(0.f, 0.f, 0.f);

		for (uint32_t i = 0; i < mesh_data.size(); ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];

			// Fill in the vertices
			for (uint32_t j = 0; j < mesh->mNumVertices; ++j)
			{
				const aiVector3D& pos = mesh->mVertices[j];

				positions.emplace_back(glm::vec3(pos.x, pos.y, pos.z));

				const aiVector3D& tex_coord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero_vector;
				tex_coords.emplace_back(glm::vec2(tex_coord.x, tex_coord.y));
				
				if (mesh->HasNormals())
				{
					const aiVector3D& norm = mesh->mNormals[j];
					normals.emplace_back(glm::vec3(norm.x, norm.y, norm.z));
				}
				else
				{
					normals.emplace_back(glm::vec3(0));
				}

				if (mesh->HasTangentsAndBitangents())
				{
					const aiVector3D& tan = mesh->mTangents[j];
					const aiVector3D& bitan = mesh->mBitangents[j];

					tangents.emplace_back(glm::vec3(tan.x, tan.y, tan.z));
					bitangents.emplace_back(glm::vec3(bitan.x, bitan.y, bitan.z));
				}
				else
				{
					tangents.emplace_back(glm::vec3(0));
					bitangents.emplace_back(glm::vec3(0));
				}
			}

			for (uint32_t j = 0; j < mesh->mNumFaces; ++j)
			{
				const aiFace& face = mesh->mFaces[j];
				CC_ASSERT(face.mNumIndices == 3, "Mesh is not triangulated by Assimp.");

				indices.emplace_back(face.mIndices[0]);
				indices.emplace_back(face.mIndices[1]);
				indices.emplace_back(face.mIndices[2]);
			}

		}

		auto slash_index = file_path.find_last_of("/");
		std::string directory;

		if (slash_index == std::string::npos)
			directory = ".";
		else if (slash_index == 0)
			directory = "/";
		else
			directory = file_path.substr(0, slash_index);
		
		mesh->materials.resize(scene->mNumMaterials);

		//for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
		//{
		//	const aiMaterial* mat = scene->mMaterials[i];
		//	
		//	textures[i] = nullptr;
		//	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		//	{
		//		aiString path;
		//		if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
		//		{
		//			std::string p = path.data;

		//			if (p.substr(0, 2) == ".\\")
		//				p = p.substr(2, p.size() - 2);

		//			std::string full_path = directory + "/" + p;

		//			textures[i] = Texture::LoadTexture(full_path.data());

		//			if (!textures[i])
		//				CC_FATAL("Texture not loaded in Assimp Loading. File path: ", full_path);
		//			else
		//				CC_TRACE("Texture loaded successfully. File path: ", full_path);
		//		}
		//	}
		//}



		// Now, buffer the data.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->attributeBuffers[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->attributeBuffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->attributeBuffers[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->attributeBuffers[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->attributeBuffers[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->attributeBuffers[5]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bitangents), bitangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		mesh->indexSize = indice_count;
		mesh->mode = Mesh::DRAW_MODE::DRAW_TRIANGLES;
		return mesh;
	}
	else
	{
		CC_FATAL("Mesh could not be loaded using Assimp.");
		return nullptr;
	}

}

void MeshBuilder::FillBuffers(Mesh* ref, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	glBindVertexArray(ref->vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, ref->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // 1st attribute buffer : positions
	glEnableVertexAttribArray(1); // 2nd attribute buffer : colors
	glEnableVertexAttribArray(2); // 3nd attribute buffer : normals 
	glEnableVertexAttribArray(3); // 4nd attribute buffer : uvs 
	glEnableVertexAttribArray(4); // 4nd attribute buffer : uvs 
	glEnableVertexAttribArray(5); // 4nd attribute buffer : uvs 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec3)));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec3) + sizeof(glm::vec2)));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec3)+ sizeof(glm::vec2) + sizeof(glm::vec3)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ref->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}




