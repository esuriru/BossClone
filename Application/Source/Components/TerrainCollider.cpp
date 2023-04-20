#include "TerrainCollider.h"
#include "Terrain.h"
#include "TerrainVolume.h"


TerrainCollider::TerrainCollider(GameObject* go)
	: Collider(go)
{
	boundingVolume = new TerrainVolume(this);
	boundingVolume->SetPosition(go->GetTransform()->GetPosition());
}

void TerrainCollider::Update(double dt)
{
	if (isDirty_)
	{
		boundingVolume->SetPosition(gameObject_->GetTransform()->GetPosition());
		isDirty_ = false;
	}
}

float TerrainCollider::FindHeight(float x, float z) const
{
	return volume_->GetHeight(x, z);
}

