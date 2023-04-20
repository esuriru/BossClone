#include "Ray.h"
#include "RayVolume.h"
#include "Physics/ColliderManager.h"
#include "Scene/GameObject.h"
#include "Transform.h"

Ray::Ray(GameObject* go)
	: Collider(go)
{
	volume_ = new RayVolume(this);
	boundingVolume = volume_;
}


void Ray::Update(double dt)
{
	//boundingVolume->SetPosition(_gameObject->GetTransform()->GetPosition());
}

std::vector<Collider*> Ray::Raycast() const
{
	static ColliderManager* cm = ColliderManager::GetInstance();
	return cm->RetrieveCollisionData(this);
}

void Ray::SetDirection(const glm::vec3& dir)
{
	volume_->direction = dir;
}

const glm::vec3& Ray::GetDirection() const
{
	return volume_->direction;
}

