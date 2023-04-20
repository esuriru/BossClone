#include "ModelRenderer.h"
#include "Renderer/ShaderLibrary.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Core/Core.h"
#include "Utils/Vertex.h"
#include <sstream>
#include "Renderer/MeshBuilder.h"
#include "Renderer/SceneRenderer.h"
#include "Transform.h"
#include "Scene/GameObject.h"

ModelRenderer::ModelRenderer(GameObject* go)
	: Renderer(go)
	, targetTransform_(go->GetComponent<Transform>())
	, loaded_(false)
{
	gameObject_->SetRenderer(this);
	renderOrder = 0;
}

ModelRenderer::~ModelRenderer()
{
	if (!loaded_)
		return;
	for (auto& i : meshes)
	{
		for (auto& j : i->materials)
		{
			delete j;
			j = nullptr;
		}
		delete i;
		i = nullptr;
	}
	meshes.clear();
}

void ModelRenderer::Update(double dt)
{
}

void ModelRenderer::Render()
{
	static SceneRenderer* renderer = SceneRenderer::GetInstance();
	for (const auto& i : meshes)
	{
		if (i)
			renderer->RenderMesh(i, targetTransform_->GetTransformMatrix());
	}
}

void ModelRenderer::ReferenceModelRenderer(ModelRenderer* md)
{
	if (!md)
		return;
	meshes = md->meshes;
	directory_ = md->directory_;

	// Delete it if it exists.
	if (loaded_)
	{
		if (!meshes.empty())
		{
			for (const auto& i : meshes)
			{
				for (const auto& j : i->materials)
				{
					delete j;
				}
				delete i;
			}
		}
	}
	loaded_ = false;
}

std::vector<Texture*> ModelRenderer::RetrieveTextures(std::vector<Texture*>& loaded_textures, aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture*> textures;
	for (uint32_t i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skipLoad = false;
		for (unsigned int j = 0; j < loaded_textures.size(); j++)
		{
			if (std::strcmp(loaded_textures[j]->GetPath().data(), str.C_Str()) == 0)
			{
				textures.emplace_back(loaded_textures[j]);
				skipLoad = true;
				break;
			}
		}

		if (!skipLoad)
		{
			std::stringstream ss;
			ss << directory_ << '/' << str.C_Str();
			const auto& texture = Texture::LoadTexture(ss.str().data());
			if (texture)
				textures.emplace_back(texture);
			else
				CC_FATAL("Texture could not be loaded. Path tried: ", ss.str());
		}
	}

	return textures;
}

std::vector<Mesh*>& ModelRenderer::LoadModel(const std::string& file_path)
{
	CC_ASSERT(meshes.empty(), "Model already loaded.");
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file_path, CC_ASSIMP_LOAD_FLAGS);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		CC_ERROR("Assimp error: ", importer.GetErrorString());
		return meshes;
	}

	directory_ = file_path.substr(0, file_path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
	loaded_ = true;
	return meshes;
}

void ModelRenderer::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(InstantiateMesh(mesh, scene));
	}
	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene);
	}

}

Mesh* ModelRenderer::InstantiateMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertex_buffer_data;
	std::vector<uint32_t> index_buffer_data;
	std::vector<Texture*> loaded_textures;

	for(uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector{}; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.pos = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x; 
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoord = vec;
			// tangent
			
		}
		else
			vertex.texCoord = glm::vec2(0.0f, 0.0f);

		if (mesh->HasTangentsAndBitangents())
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		}


		vertex_buffer_data.push_back(vertex);
	}
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		CC_ASSERT(face.mNumIndices == 3, "Mesh not triangulated!");

		for (uint32_t j = 0; j < face.mNumIndices; j++)
			index_buffer_data.push_back(face.mIndices[j]);        
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<Texture*> diffuseMaps = RetrieveTextures(loaded_textures, material, aiTextureType_DIFFUSE, "u_diffuse_texture");
	loaded_textures.insert(loaded_textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture*> specularMaps = RetrieveTextures(loaded_textures, material, aiTextureType_SPECULAR, "u_specular_texture");
	loaded_textures.insert(loaded_textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture*> normalMaps = RetrieveTextures(loaded_textures, material, aiTextureType_HEIGHT, "u_normal_texture");
	loaded_textures.insert(loaded_textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture*> heightMaps = RetrieveTextures(loaded_textures, material, aiTextureType_AMBIENT, "u_height_texture");
	loaded_textures.insert(loaded_textures.end(), heightMaps.begin(), heightMaps.end());

	aiColor3D ambient;
	material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

	aiColor3D diff;
	material->Get(AI_MATKEY_COLOR_DIFFUSE, diff);

	aiColor3D spec;
	material->Get(AI_MATKEY_COLOR_SPECULAR, spec);

	float shininess;
	material->Get(AI_MATKEY_SHININESS, shininess);

	int illumModel = 1;
	material->Get(AI_MATKEY_SHADING_MODEL, illumModel);
	
	// For now handle only one.
	Material* newMat = new Material();
	newMat->materialShader = ShaderLibrary::GetInstance()->Get("Experimental Blinn Phong");
	newMat->textureMaps[Material::AMBIENT] = heightMaps.empty() ? nullptr : heightMaps.front();
	newMat->textureMaps[Material::DIFFUSE] = diffuseMaps.empty() ? nullptr : diffuseMaps.front();
	newMat->textureMaps[Material::SPECULAR] = specularMaps.empty() ? nullptr : specularMaps.front();
	newMat->textureMaps[Material::NORMAL] = normalMaps.empty() ? nullptr : normalMaps.front();
	
	newMat->data->_ambient = glm::vec4(ambient.r, ambient.g, ambient.b, 1);
	newMat->data->_diffuse = glm::vec4(diff.r, diff.g, diff.b, 1);
	newMat->data->_specular = glm::vec4(spec.r, spec.g, spec.b, 1);
	newMat->data->_shininess = shininess;

	switch (illumModel)
	{
	case aiShadingMode_Blinn:
	case aiShadingMode_Phong:
		newMat->materialShader = ShaderLibrary::GetShaderFromIllumModel(2);
		break;
	case aiShadingMode_Flat:
		newMat->materialShader = ShaderLibrary::GetShaderFromIllumModel(0);
		break;
	case aiShadingMode_Minnaert:
	default:
		newMat->materialShader = ShaderLibrary::GetShaderFromIllumModel(1);
		break;
	}


	// return a mesh object created from the extracted mesh data
	Mesh* temp = MeshBuilder::Generate("Assimp Model Mesh", vertex_buffer_data, index_buffer_data);
	//CC_TRACE("Last vertex: ", vertex_buffer_data.size(), " Max element: ", *std::max_element(index_buffer_data.begin(), index_buffer_data.end()));
	temp->materials.emplace_back(newMat);

	//temp->indexSize = mesh->mNumFaces * 3;
	newMat->size = temp->indexSize;
	return temp;
}

