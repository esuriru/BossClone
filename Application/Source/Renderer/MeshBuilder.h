#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Utils/Vertex.h"
#include <vector>

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateQuad(const std::string& meshName, glm::vec4 color, float length = 1.f);
	static Mesh* GenerateCube(const std::string& meshName, glm::vec4 color, float length = 1.f);
	static Mesh* GenerateRing(const std::string& meshName, glm::vec4 color, unsigned numSlice, float outerR = 1.f, float innerR = 0.f);
	static Mesh* GenerateSphere(const std::string& meshName, glm::vec4 color, unsigned numStack, unsigned numSlice, float radius = 1.f);
	static Mesh* GenerateCone(const std::string& meshName, glm::vec4 color, unsigned numSlice, float radius, float height);
	static Mesh* GenerateTriangle(const std::string& meshName, glm::vec4 color, float length = 1.f);
	static Mesh* GenerateText(const std::string& meshName, unsigned row, unsigned col);
	static Mesh* GenerateOBJ(const std::string& meshName, const std::string& file_path);
	static Mesh* GenerateOBJMTL(const std::string& meshName,  const std::string& file_path, const std::string& mtl_path);
	static Mesh* GenerateMappedOBJ(const std::string& meshName,  const std::string& file_path, const std::string& mtl_path);
	static Mesh* GenerateMappedOBJ(const std::string& meshName, const std::string& obj_path);
	static Mesh* Generate(const std::string& meshName, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
#if 0
	static Mesh* GenerateTerrain(const std::string& meshName, const std::string& height_map, const std::string& textureMaps[Material::DIFFUSE]);
#endif
	static Mesh* LoadStaticMesh(const std::string& name, const std::string& file_path);
	
private:
	static void FillBuffers(Mesh* ref, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

};


#endif