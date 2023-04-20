#pragma once
#include "Component.h"
#include "Renderer/Mesh.h"
class MeshFilter : public Component
{
private:
	Mesh* mesh_;
public:
	void SetMesh(Mesh* m);
	Mesh* GetMesh(void) const;

	MeshFilter(GameObject* go, Mesh* mesh = nullptr);
	virtual ~MeshFilter();

	void Update(double dt);
};

