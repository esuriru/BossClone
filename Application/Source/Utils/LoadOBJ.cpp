#include <iostream>
#include <glm/gtx/norm.hpp>
#include "Renderer/ShaderLibrary.h"
#include <fstream>
#include <map>

#include "LoadOBJ.h"
#include <sstream>
#include <string>
#include "Renderer/Material.h"

struct PackedVertex {
	glm::vec3 centre;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const {
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
	};
};

bool is_near(float v1, float v2){
	return fabs( v1-v2 ) < 0.01f;
}

bool getSimilarVertexIndex(
	PackedVertex v1,
	std::map<PackedVertex, unsigned short>& VertexToOutIndex,
	unsigned short & result,
	PackedVertex& resultVertex
)
{
	for (const auto& v2 : VertexToOutIndex)
	{
		if (
			is_near( v1.centre.x , v2.first.centre.x ) &&
			is_near( v1.centre.y , v2.first.centre.y ) &&
			is_near( v1.centre.z , v2.first.centre.z ) &&
			is_near( v1.uv.x     ,	 v2.first.uv.x ) &&
			is_near( v1.uv.y     ,   v2.first.uv.y ) &&
			is_near( v1.normal.x ,   v2.first.normal.x ) &&
			is_near( v1.normal.y ,   v2.first.normal.y ) &&
			is_near( v1.normal.z ,   v2.first.normal.z )
		)
		{
			result = v2.second;
			return true;
		}
	}
	return false;
}


bool LoadOBJ(
	const char* file_path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	//Fill up code from OBJ lecture notes
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open())
	{
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return false;
	}
	std::vector<unsigned> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::string line;
	while (std::getline(fileStream, line)) {

		if (line.substr(0, 2) == "v ") {
			// process vertex position
			std::istringstream data(line.substr(2));
			glm::vec3 vertice;
			data >> vertice.x >> vertice.y >> vertice.z;
			temp_vertices.emplace_back(vertice);
		}
		else if (line.substr(0, 2) == "vt") { 
			// process texcoord
			std::istringstream data(line.substr(3));
			glm::vec2 texCoord;
			data >> texCoord.x >> texCoord.y;
			temp_uvs.emplace_back(texCoord);
		}
		else if (line.substr(0, 2) == "vn") {
			// process normal
			std::istringstream data(line.substr(3));
			glm::vec3 normal;
			data >> normal.x >> normal.y >> normal.z;
			temp_normals.emplace_back(normal);
		}
		else if (line.substr(0, 2) == "f ")
		{
			// process face
			for (int i = 0; i < 2; ++i)
			{
				unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
				int matches = sscanf_s(line.data(), i ? "f %d//%d//%d %d//%d//%d %d//%d//%d %d//%d//%d\n" :"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2],
					&vertexIndex[3], &uvIndex[3], &normalIndex[3]);

				if (matches == 9) //triangle (hint: index 0,1,2)
				{
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
				else if (matches == 12) //quad (hint: index 0,1,2 & 0,2,3)
				{
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[2]);
					vertexIndices.push_back(vertexIndex[3]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[2]);
					uvIndices.push_back(uvIndex[3]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[2]);
					normalIndices.push_back(normalIndex[3]);

				}
				else
				{
					unsigned int indices[4];
					int second_matches = sscanf_s(line.data(), i ? "f %d//%d %d//%d %d//%d %d//%d\n" : "f %d/%d %d/%d %d/%d %d/%d\n",
						&vertexIndex[0], &indices[0],
						&vertexIndex[1], &indices[1],
						&vertexIndex[2], &indices[2],
						&vertexIndex[3], &indices[3]);
					if (second_matches == 6)
					{
						if (temp_uvs.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							normalIndices.push_back(indices[0]);
							normalIndices.push_back(indices[1]);
							normalIndices.push_back(indices[2]);
						}
						else if (temp_normals.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							uvIndices.push_back(indices[0]);
							uvIndices.push_back(indices[1]);
							uvIndices.push_back(indices[2]);
						}
					}
					else
						continue;
				}

			}
		}
	}
	fileStream.close(); // close file

	// For each vertex of each triangle, after fileStream.close()
	for (unsigned i = 0; i < vertexIndices.size(); ++i)
	{
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = 0;
		if (!temp_uvs.empty())
			uvIndex = uvIndices[i];

		unsigned int normalIndex = 0;

		if (!temp_normals.empty())
			normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv;
		if (!temp_uvs.empty())
			uv = temp_uvs[uvIndex - 1];

		glm::vec3 normal;

		if (!temp_normals.empty())
			normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		if (!temp_uvs.empty())
			out_uvs.push_back(uv);

		if (!temp_normals.empty())
			out_normals.push_back(normal);
	}

	return true;
}


bool getSimilarVertexIndex_fast(
	PackedVertex& packed,
	std::map<PackedVertex, unsigned short>& VertexToOutIndex,
	unsigned short& result
) {
	std::map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end())
	{
		return false;
	}
	else
	{
		result = it->second;
		return true;
	}
}

void IndexVBO(
	std::vector<glm::vec3>& in_vertices,
	std::vector<glm::vec2>& in_uvs,
	std::vector<glm::vec3>& in_normals,

	std::vector<unsigned>& out_indices,
	std::vector<Vertex>& out_vertices
)
{
	std::map<PackedVertex, unsigned short> VertexToOutIndex;

	std::vector<glm::vec2> uvs = in_uvs;
	if (in_uvs.empty())
		for (int i = 0; i < in_vertices.size(); ++i)
			uvs.push_back({ 0, 0 });
	std::vector<glm::vec3> normals = in_normals;
	if (in_normals.empty())
		for (int i = 0; i < in_vertices.size(); ++i)
			normals.push_back({ 0, 0,0  });


	// For each input vertex
	for (unsigned int i = 0; i < in_vertices.size(); ++i)
	{

		PackedVertex packed = { in_vertices[i], uvs[i], normals[i] };

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);

		if (found)
		{
			// A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
		}
		else
		{
			// If not, it needs to be added in the output data.
			Vertex v;
			v.pos = in_vertices[i];
			v.texCoord = glm::vec2(uvs[i].s, uvs[i].t);
			v.normal = normals[i];
			v.color = { 1, 1, 1, 1 };
			out_vertices.push_back(v);
			unsigned newindex = (unsigned)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}

void IndexVBO_TBN(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	std::vector<glm::vec3> & in_normals,
	std::vector<glm::vec3> & in_tangents,
	std::vector<glm::vec3> & in_bitangents,

	std::vector<unsigned>& out_indices,
	std::vector<Vertex>& out_vertices

	//std::vector<unsigned short> & out_indices,
	//std::vector<glm::vec3> & out_vertices,
	//std::vector<glm::vec2> & out_uvs,
	//std::vector<glm::vec3> & out_normals,
	//std::vector<glm::vec3> & out_tangents,
	//std::vector<glm::vec3> & out_bitangents
){

	std::vector<glm::vec2> uvs = in_uvs;
	if (in_uvs.empty())
		for (int i = 0; i < in_vertices.size(); ++i)
			uvs.push_back({ 0, 0 });
	std::vector<glm::vec3> normals = in_normals;
	if (in_normals.empty())
	{
		for (int i = 0; i < in_vertices.size(); ++i)
			normals.push_back({ 0, 0,0  });
	}

	if (in_tangents.empty())
	{
		for (int i = 0; i < in_vertices.size(); ++i)
			in_tangents.push_back({ 0, 0,0  });
	}

	if (in_bitangents.empty())
	{
		for (int i = 0; i < in_vertices.size(); ++i)
			in_bitangents.push_back({ 0, 0,0  });
	}
	

	// https://github.com/opengl-tutorials/ogl/blob/master/common/vboindexer.cpp

	std::map<PackedVertex, unsigned short> VertexToOutIndex;
	// For each input vertex
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		PackedVertex vert = { in_vertices[i], in_uvs[i], in_normals[i] };
		PackedVertex result;
		bool found = getSimilarVertexIndex(vert, VertexToOutIndex, index, result);

		if (found)
		{
			out_indices.push_back( index );

			out_vertices[index].tangent += in_tangents[i];
			out_vertices[index].bitangent += in_bitangents[i];

			//// Average the tangents and the bitangents
			//out_tangents[index] += in_tangents[i];
			//out_bitangents[index] += in_bitangents[i];
		}
		else
		{
			Vertex v;
			v.pos = in_vertices[i];
			v.texCoord = glm::vec2(uvs[i].s, uvs[i].t);
			v.normal = normals[i];
			v.color = { 1, 1, 1, 1 };
			v.tangent = in_tangents[i];
			v.bitangent = in_bitangents[i];

			out_vertices.push_back(v);
			unsigned newindex = (unsigned)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[vert] = newindex;
		}
	}
}


bool LoadMTL(const char* file_path, std::map<std::string, Material*>& materials_map)
{
	std::ifstream fileStream(file_path, std::ios::in | std::ios::binary);

	if (!fileStream.is_open())
	{
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return false;
	}

	Material* mtl = nullptr;
		
	std::string file_path_string = file_path;
	auto lastSlash = file_path_string.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

	std::string directory = file_path_string.substr(0, lastSlash);

	std::string line;

	while (std::getline(fileStream, line))
	{
		if (line.substr(0, 7) == "newmtl ") { //process newmtl
			std::istringstream data(line.substr(7));
			std::string name;
			//std::getline(data, name);
			data >> name;
			mtl = nullptr;
			if (materials_map.find(name) == materials_map.end())
			{
				mtl = new Material();
				materials_map.insert(std::pair<std::string, Material*>(name, mtl));
			}
		}
		else if (line.substr(0, 3) == "Ka ")
		{ //process Ka
			if (mtl != nullptr)
			{
				std::string data = line.substr(3);
				//if (data.find('/\\') != std::string::npos)
				//{
				//	// It is a path
				//	mtl->textureMaps[AMBIENT_MAPPED] = Texture::LoadTexture(data.data());
				//}
				//else
				//{
					int success_rate = sscanf_s(data.data(), "%f%f%f", &mtl->data->_ambient.r, &mtl->data->_ambient.g, &mtl->data->_ambient.b);
				//}

			}
		}
		else if (line.substr(0, 3) == "Kd ") 
		{ //process Kd
			if (mtl != nullptr)
			{
				std::string data = line.substr(3);
				//if (data.find('/\\') != std::string::npos)
				//{
				//	// It is a path
				//	mtl->textureMaps[DIFFUSE_MAPPED] = Texture::LoadTexture(data.data());
				//}
				//else
				//{
					sscanf_s(data.data(), "%f%f%f", &mtl->data->_diffuse.r, &mtl->data->_diffuse.g, &mtl->data->_diffuse.b);
				//}
			}
		}
		else if (line.substr(0, 3) == "Ks ") 
		{ //process Ks
			if (mtl != nullptr)
			{
				std::string data = line.substr(3);
				sscanf_s(data.data(), "%f%f%f", &mtl->data->_specular.r, &mtl->data->_specular.g, &mtl->data->_specular.b);
			}
		}
		else if (line.substr(0, 3) == "Ns ")
		{ //process Ns
			if (mtl != nullptr)
			{
				std::string data = line.substr(3);
				sscanf_s(data.data(), "%f", &mtl->data->_shininess);
			}
		}
		else if (line.substr(0, 6) == "illum ")
		{
			if (mtl != nullptr)
			{
				int im = std::stoi(line.substr(6));
				mtl->illuminationModel = im;
			}
		}
		else if (line.substr(0, 7) == "map_Kd ")
		{
			//std::istringstream data(line.substr(7));
			//std::string path;
			//data >> path;
			//if (path.find('/\\') != std::string::npos)
			//{
			// It is a path
			//mtl->textureMaps[Material::DIFFUSE] = Texture::LoadTexture(path.data());
			//}
		}

		else if (line.substr(0, 7) == "map_Ka ")
		{
			//std::istringstream data(line.substr(7));
			//std::string path;
			//data >> path;
			//mtl->textureMaps[Material::AMBIENT] = Texture::LoadTexture(path.data());
		}

		else if (line.substr(0, 7) == "map_Ks ")
		{
			//std::istringstream data(line.substr(7));
			//std::string path;
			//data >> path;
			//mtl->textureMaps[Material::SPECULAR] = Texture::LoadTexture(path.data());
		}

		else if (line.substr(0, 9) == "map_Bump ")
		{
			//std::istringstream data(line.substr(9));
			//std::string path;
			//data >> path;

			//float bump_multiplier = 1.0f;
			//char file_path[512];
			//sscanf_s(path.data(), "-bm %f %[^\n]s", &bump_multiplier, file_path);
			//
			//mtl->textureMaps[NORMAL_MAPPED] = Texture::LoadTexture(directory.append(file_path).data());
			//mtl->bumpMultiplier = bump_multiplier;
		}
	}
	fileStream.close(); // close file

	return true;
}


bool LoadOBJMTL(const char* file_path, const char* mtl_path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals, std::vector<Material*>& out_materials)
{

	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open())
	{
		CC_FATAL("Impossible to open ",file_path, ". Are you in the right directory ?");
		return false;
	}
	std::vector<unsigned> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::map<std::string, Material*> materials_map;
	if(mtl_path != nullptr && !LoadMTL(mtl_path, materials_map))
		return false;

	std::string line;
	while (std::getline(fileStream, line)) {
		if (line.substr(0, 2) == "v ") {
			// process vertex position
			std::istringstream data(line.substr(2));
			glm::vec3 vertice;
			data >> vertice.x >> vertice.y >> vertice.z;
			temp_vertices.emplace_back(vertice);
		}
		else if (line.substr(0, 2) == "vt") { 
			// process texcoord
			std::istringstream data(line.substr(3));
			glm::vec2 texCoord;
			data >> texCoord.x >> texCoord.y;
			temp_uvs.emplace_back(texCoord);
		}
		else if (line.substr(0, 2) == "vn") {
			// process normal
			std::istringstream data(line.substr(3));
			glm::vec3 normal;
			data >> normal.x >> normal.y >> normal.z;
			if (glm::length2(normal) < 0.01f)
				CC_TRACE("Normal with close to or zero length detected.");
			temp_normals.emplace_back(normal);
		}
		else if (line.substr(0, 2) == "f ")
		{
			// process face
			for (int i = 0; i < 2; ++i)
			{
				unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
				int matches = sscanf_s(line.data(), i ? "f %d//%d//%d %d//%d//%d %d//%d//%d %d//%d//%d\n" :"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2],
					&vertexIndex[3], &uvIndex[3], &normalIndex[3]);

				if (matches == 9) //triangle (hint: index 0,1,2)
				{
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

					if (!out_materials.empty())
						out_materials.back()->size += 3;
				}
				else if (matches == 12) //quad (hint: index 0,1,2 & 0,2,3)
				{
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[2]);
					vertexIndices.push_back(vertexIndex[3]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[2]);
					uvIndices.push_back(uvIndex[3]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[2]);
					normalIndices.push_back(normalIndex[3]);

					if (!out_materials.empty())
						out_materials.back()->size += 6;

				}
				else
				{
					unsigned int indices[4];
					int second_matches = sscanf_s(line.data(), i ? "f %d//%d %d//%d %d//%d %d//%d\n" : "f %d/%d %d/%d %d/%d %d/%d\n",
						&vertexIndex[0], &indices[0],
						&vertexIndex[1], &indices[1],
						&vertexIndex[2], &indices[2],
						&vertexIndex[3], &indices[3]);
					if (second_matches == 6)
					{
						if (temp_uvs.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							normalIndices.push_back(indices[0]);
							normalIndices.push_back(indices[1]);
							normalIndices.push_back(indices[2]);

						}
						else if (temp_normals.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							uvIndices.push_back(indices[0]);
							uvIndices.push_back(indices[1]);
							uvIndices.push_back(indices[2]);
						}

						if (!out_materials.empty())
							out_materials.back()->size += 3;
					}
					else if (second_matches == 8)
					{
						if (temp_uvs.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							normalIndices.push_back(indices[0]);
							normalIndices.push_back(indices[1]);
							normalIndices.push_back(indices[2]);

							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[2]);
							vertexIndices.push_back(vertexIndex[3]);
							normalIndices.push_back(indices[0]);
							normalIndices.push_back(indices[2]);
							normalIndices.push_back(indices[3]);

						}
						else if (temp_normals.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							uvIndices.push_back(indices[0]);
							uvIndices.push_back(indices[1]);
							uvIndices.push_back(indices[2]);

							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[2]);
							vertexIndices.push_back(vertexIndex[3]);
							uvIndices.push_back(indices[0]);
							uvIndices.push_back(indices[2]);
							uvIndices.push_back(indices[3]);
						}
						if (!out_materials.empty())
							out_materials.back()->size += 6;
					}
					else
						continue;
				}

			}
		}
		//else if (strncmp("mtllib ", buf, 7) == 0) { //process mtllib
		//	char mtl_path[256];
		//	strcpy_s(mtl_path, buf + 7);
		//	LoadMTL(mtl_path, materials_map);
		//}

		else if (line.substr(0, 7) == "usemtl ") {
			// process usemtl
			std::string mtl_name;
			std::istringstream data(line.substr(7));
			data >> mtl_name;
			if (materials_map.find(mtl_name) != materials_map.end())
			{
				Material* mtl = materials_map[mtl_name];
				out_materials.push_back(mtl);
			}
		}
		
	}
	fileStream.close(); // close file

	for (unsigned i = 0; i < vertexIndices.size(); ++i)
	{
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = 0;
		if (!temp_uvs.empty())
			uvIndex = uvIndices[i];

		unsigned int normalIndex = 0;

		if (!temp_normals.empty())
			normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv;
		if (!temp_uvs.empty())
			uv = temp_uvs[uvIndex - 1];

		glm::vec3 normal;

		if (!temp_normals.empty())
			normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		if (!temp_uvs.empty())
			out_uvs.push_back(uv);

		if (!temp_normals.empty())
			out_normals.push_back(normal);

	}
	for (auto& i : out_materials)
	{
		i->materialShader = ShaderLibrary::GetShaderFromIllumModel(i->illuminationModel);
		//if (i->illuminationModel == 2)
		//	i->data->_colour = { i->data->_diffuse.x,i->data->_diffuse.y,i->data->_diffuse.z, 1 };
		//	i->data->_colour = { i->data->_diffuse.x,i->data->_diffuse.y,i->data->_diffuse.z, 1 };
		//i->data->_colour = { 0, 0, 0, 1 };
		//i->data->_colour = { 0.1f, 0.1f, 0.1f, 1 };
		//i->data->_colour = { i->data->_ambient.x,i->data->_ambient.y,i->data->_ambient.z, 1 };
		//i->data->_colour = { i->data->_diffuse.x,i->data->_diffuse.y,i->data->_diffuse.z, 1 };
	}


	//for (std::map<std::string, Material*>::iterator it = materials_map.begin(); it != materials_map.end(); ++it)
	//{
	//	delete it->second;
	//}
	materials_map.clear();

	return true;
}

void computeTangentBasis(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents)
{
	if (normals.empty() || uvs.empty())
	{
		std::cout << "Normals or UVs empty. Check here if model did not load properly.";
		return;
	}

	for (int i = 0; i < vertices.size(); i += 3) {

		// Shortcuts for vertices
		glm::vec3& v0 = vertices[i + 0];
		glm::vec3& v1 = vertices[i + 1];
		glm::vec3& v2 = vertices[i + 2];

		// Shortcuts for UVs
		glm::vec2& uv0 = uvs[i + 0];
		glm::vec2& uv1 = uvs[i + 1];
		glm::vec2& uv2 = uvs[i + 2];

		// Edges of the triangle : position delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

		// Set the same tangent for all three vertices of the triangle.
		// They will be merged later, in vboindexer.cpp
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		// Same thing for bitangents
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}
}

bool LoadMappedOBJ(const char* file_path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals, std::vector<glm::vec3>& out_tangents, std::vector<glm::vec3>& out_bitangents, std::vector<Material*>& out_materials)
{
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open())
	{
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return false;
	}
	std::vector<unsigned> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::map<std::string, Material*> materials_map;
	//if(mtl_path != nullptr && !LoadMTL(mtl_path, materials_map))
	//	return false;

	std::string line;
	while (std::getline(fileStream, line)) {
		if (line.substr(0, 2) == "v ") {
			// process vertex position
			std::istringstream data(line.substr(2));
			glm::vec3 vertice;
			data >> vertice.x >> vertice.y >> vertice.z;
			temp_vertices.emplace_back(vertice);
		}
		else if (line.substr(0, 2) == "vt") { 
			// process texcoord
			std::istringstream data(line.substr(3));
			glm::vec2 texCoord;
			data >> texCoord.x >> texCoord.y;
			temp_uvs.emplace_back(texCoord);
		}
		else if (line.substr(0, 2) == "vn") {
			// process normal
			std::istringstream data(line.substr(3));
			glm::vec3 normal;
			data >> normal.x >> normal.y >> normal.z;
			temp_normals.emplace_back(normal);
		}
		else if (line.substr(0, 2) == "f ")
		{
			// process face
			for (int i = 0; i < 2; ++i)
			{
				unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
				int matches = sscanf_s(line.data(), i ? "f %d//%d//%d %d//%d//%d %d//%d//%d %d//%d//%d\n" :"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2],
					&vertexIndex[3], &uvIndex[3], &normalIndex[3]);

				if (matches == 9) //triangle (hint: index 0,1,2)
				{
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

					if (!out_materials.empty())
						out_materials.back()->size += 3;
				}
				else if (matches == 12) //quad (hint: index 0,1,2 & 0,2,3)
				{
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[2]);
					vertexIndices.push_back(vertexIndex[3]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[2]);
					uvIndices.push_back(uvIndex[3]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[2]);
					normalIndices.push_back(normalIndex[3]);

					if (!out_materials.empty())
						out_materials.back()->size += 6;

				}
				else
				{
					unsigned int indices[4];
					int second_matches = sscanf_s(line.data(), i ? "f %d//%d %d//%d %d//%d %d//%d\n" : "f %d/%d %d/%d %d/%d %d/%d\n",
						&vertexIndex[0], &indices[0],
						&vertexIndex[1], &indices[1],
						&vertexIndex[2], &indices[2],
						&vertexIndex[3], &indices[3]);
					if (second_matches == 6)
					{
						if (temp_uvs.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							normalIndices.push_back(indices[0]);
							normalIndices.push_back(indices[1]);
							normalIndices.push_back(indices[2]);

						}
						else if (temp_normals.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							uvIndices.push_back(indices[0]);
							uvIndices.push_back(indices[1]);
							uvIndices.push_back(indices[2]);
						}

						if (!out_materials.empty())
							out_materials.back()->size += 3;
					}
					else if (second_matches == 8)
					{
						if (temp_uvs.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							normalIndices.push_back(indices[0]);
							normalIndices.push_back(indices[1]);
							normalIndices.push_back(indices[2]);

							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[2]);
							vertexIndices.push_back(vertexIndex[3]);
							normalIndices.push_back(indices[0]);
							normalIndices.push_back(indices[2]);
							normalIndices.push_back(indices[3]);

						}
						else if (temp_normals.empty())
						{
							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[1]);
							vertexIndices.push_back(vertexIndex[2]);
							uvIndices.push_back(indices[0]);
							uvIndices.push_back(indices[1]);
							uvIndices.push_back(indices[2]);

							vertexIndices.push_back(vertexIndex[0]);
							vertexIndices.push_back(vertexIndex[2]);
							vertexIndices.push_back(vertexIndex[3]);
							uvIndices.push_back(indices[0]);
							uvIndices.push_back(indices[2]);
							uvIndices.push_back(indices[3]);
						}
						if (!out_materials.empty())
							out_materials.back()->size += 6;
					}
					else
						continue;
				}

			}
		}
		else if (line.substr(0, 7) == "mtllib ")
		// Might not use the first material at the start.
		{
			std::string file_path_string = file_path;
			auto lastSlash = file_path_string.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

			std::string directory = file_path_string.substr(0, lastSlash);

			std::string path;
			std::istringstream data(line.substr(7));
			data >> path;
			LoadMTL(directory.append(path).data(), materials_map);
		}
				//else if (strncmp("mtllib ", buf, 7) == 0) { //process mtllib
		//	char mtl_path[256];
		//	strcpy_s(mtl_path, buf + 7);
		//	LoadMTL(mtl_path, materials_map);
		//}
		else if (line.substr(0, 7) == "usemtl ") {
			// process usemtl
			std::string mtl_name;
			std::istringstream data(line.substr(7));
			data >> mtl_name;
			if (materials_map.find(mtl_name) != materials_map.end())
			{
				Material* mtl = materials_map[mtl_name];
				out_materials.push_back(mtl);
			}
		}
		
	}
	fileStream.close(); // close file

	for (unsigned i = 0; i < vertexIndices.size(); ++i)
	{
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = 0;
		if (!temp_uvs.empty())
			uvIndex = uvIndices[i];

		unsigned int normalIndex = 0;

		if (!temp_normals.empty())
			normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv;
		if (!temp_uvs.empty())
			uv = temp_uvs[uvIndex - 1];

		glm::vec3 normal;

		if (!temp_normals.empty())
			normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		if (!temp_uvs.empty())
			out_uvs.push_back(uv);

		if (!temp_normals.empty())
			out_normals.push_back(normal);

	}
	for (auto& i : out_materials)
	{
		i->materialShader = ShaderLibrary::GetShaderFromIllumModel(i->illuminationModel);
		//if (i->illuminationModel == 2)
		//	i->data->_colour = { i->data->_diffuse.x,i->data->_diffuse.y,i->data->_diffuse.z, 1 };
	}

	materials_map.clear();
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	computeTangentBasis(out_vertices, out_uvs, out_normals, tangents, bitangents);
	out_tangents = tangents;
	out_bitangents = bitangents;
	
	return true;
}


