#include "UIRenderer.h"
#include "Renderer/SceneRenderer.h"
#include "Renderer/Mesh.h"
#include "Transform.h"

UIRenderer::UIRenderer(GameObject* go)
	: MeshRenderer(go)
	, billboarded(false)
{
	renderOrder = 1;
}

void UIRenderer::Render()
{
	static SceneRenderer* renderer = SceneRenderer::GetInstance();
	if (targetMesh_ != nullptr)
	{
		if (!billboarded)
			renderer->RenderMeshOnScreen(targetMesh_, targetTransform_->GetTransformMatrix());
		else
			renderer->RenderMeshBillboarded(targetMesh_, targetTransform_->GetPosition(), { targetTransform_->GetScale() });
	}
}
