#ifndef LOAD_OBJ_H
#define LOAD_OBJ_H

#include <vector>
#include <glm\glm.hpp>
#include "Vertex.h"
#include "Renderer/Material.h"

bool LoadOBJ(
	const char *file_path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals
);

void IndexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	std::vector<glm::vec3> & in_normals,

	std::vector<unsigned> & out_indices,
	std::vector<Vertex> & out_vertices
);

void IndexVBO_TBN(
	std::vector<glm::vec3>& in_vertices,
	std::vector<glm::vec2>& in_uvs,
	std::vector<glm::vec3>& in_normals,
	std::vector<glm::vec3>& in_tangents,
	std::vector<glm::vec3>& in_bitangents,

	std::vector<unsigned>& out_indices,
	std::vector<Vertex>& out_vertices
	//std::vector<unsigned short>& out_indices,
	//std::vector<glm::vec3>& out_vertices,
	//std::vector<glm::vec2>& out_uvs,
	//std::vector<glm::vec3>& out_normals,
	//std::vector<glm::vec3>& out_tangents,
	//std::vector<glm::vec3>& out_bitangents
);


bool LoadMTL(const char* file_path, std::map<std::string, Material*>& materials_map);

bool LoadOBJMTL(
	const char* file_path, 
	const char* mtl_path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals,
	std::vector<Material*>& out_materials
);

void computeTangentBasis(
	// inputs
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals,
	// outputs
	std::vector<glm::vec3>& tangents,
	std::vector<glm::vec3>& bitangents
);

bool LoadMappedOBJ(
	const char* file_path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals,
	std::vector<glm::vec3>& out_tangents,
	std::vector<glm::vec3>& out_bitangents,
	std::vector<Material*>& out_materials
);

#endif