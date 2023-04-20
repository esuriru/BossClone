#pragma once
#include "Renderer/Mesh.h"
#include <string>
#include <assimp/scene.h>
#include "Components/Renderer.h"

class Transform;

class ModelRenderer : public Renderer 
{
public:
	ModelRenderer(GameObject* go);
	~ModelRenderer() override;

	void Update(double dt);
	void Render() override;

	void ReferenceModelRenderer(ModelRenderer* md);

	std::vector<Mesh*> meshes;

	std::vector<Mesh*>& LoadModel(const std::string& file_path);
private:
	bool loaded_;
	std::string directory_;

	Transform* targetTransform_;

	std::vector<Texture*> RetrieveTextures(std::vector<Texture*>& loaded_textures, aiMaterial* mat, aiTextureType type, std::string typeName);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh* InstantiateMesh(aiMesh* mesh, const aiScene* scene);
};

