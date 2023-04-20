#include "MeshRenderer.h"
#include "Scene/GameObject.h"
#include "Transform.h"
#include "MeshFilter.h"
#include <cassert>
#include <iostream>
#include "Renderer/SceneRenderer.h"
#include "Core/Log.h"

MeshRenderer::MeshRenderer(GameObject* go)
	: Renderer(go)
	, targetMesh_(nullptr)
	, targetTransform_(go->GetTransform())
	, meshMaterial_(nullptr)
{
	CC_ASSERT(targetTransform_ != nullptr, "Should not be able to add on MeshRenderer before a Transform is added.");
	gameObject_->SetRenderer(this);
	renderOrder = 0;
}

MeshRenderer::~MeshRenderer()
{
	targetMesh_ = nullptr;
}

void MeshRenderer::Update(double dt)
{
	if (isDirty_)
	{
		const auto& mf = gameObject_->GetComponent<MeshFilter>();
		if (mf != nullptr)
		{
			targetMesh_ = mf->GetMesh();
			if (targetMesh_ && targetMesh_->materials.empty())
				targetMesh_->materials.push_back(meshMaterial_);
		}

		mf->SetDirty(false);
		isDirty_ = false;
	}
}

void MeshRenderer::Render()
{
	static SceneRenderer* renderer = SceneRenderer::GetInstance();
	if (targetMesh_ != nullptr)
	{
		renderer->RenderMesh(targetMesh_, targetTransform_->GetTransformMatrix());
	}
}

Mesh* MeshRenderer::GetMesh()
{
	return targetMesh_;
}

void MeshRenderer::SetMaterial(Material* mat)
{
	if (targetMesh_ && targetMesh_->materials.empty())
		targetMesh_->materials.push_back(mat);
	else
	{
		CC_INFO("Could not set material as target mesh is not set.");

		// Cache it.
		meshMaterial_ = mat;
	}
	isDirty_ = true;
}
