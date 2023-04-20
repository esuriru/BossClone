#include "MeshFilter.h"
#include "Scene/GameObject.h"
#include "MeshRenderer.h"

void MeshFilter::SetMesh(Mesh* m)
{
	mesh_ = m;
	isDirty_ = true;
	const auto& renderer = gameObject_->GetComponent<MeshRenderer>();
	if (renderer)
		renderer->SetDirty(true);
}

Mesh* MeshFilter::GetMesh(void) const
{
	return mesh_;
}

MeshFilter::MeshFilter(GameObject* go, Mesh* mesh)
	: Component(go),
	mesh_(mesh)
{
}

MeshFilter::~MeshFilter()
{
	mesh_ = nullptr;
}

void MeshFilter::Update(double dt)
{
	
}
