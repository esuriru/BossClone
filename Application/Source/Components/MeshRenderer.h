#pragma once
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Renderer.h"

class GameObject;
class Transform;

class MeshRenderer : public Renderer
{
protected:
	// Cached variables
	Mesh* targetMesh_;
	Transform* targetTransform_;

	Material* meshMaterial_;

public:
	MeshRenderer(GameObject* go);
	virtual ~MeshRenderer() override;

	void Update(double dt);
	virtual void Render();
	
	Mesh* GetMesh();

	void SetMaterial(Material* mat);
};

