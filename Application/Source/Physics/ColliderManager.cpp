#include "ColliderManager.h"
#include "Scene/GameObject.h"
#include <algorithm>

void ColliderManager::Update2D()
{
	if (colliders2D_.empty())
		return;

	for (auto& i : collider2DMap_)
		i.second.clear();

	for (const auto& c1 : colliders2D_)
	{
		if (!c1->IsActive() || !c1->GetGameObject()->ActiveSelf())
			continue;
		for (const auto& c2 : colliders2D_)
		{
			if (!c2->IsActive() || !c1->GetGameObject()->ActiveSelf() || c1 == c2)
				continue;

			if (c1->IsColliding(c2))
			{
				collider2DMap_[c1].emplace_back(c2);
			}
		}
	}
}

void ColliderManager::Update3D()
{
	if (colliders_.empty())
		return;

	for (auto& i : colliderMap_)
		i.second.clear();

	for (const auto& c1 : colliders_)
	{
		if (!c1->IsActive() || !c1->GetGameObject()->ActiveSelf())
			continue;
		for (const auto& c2 : colliders_)
		{
			if (!c2->IsActive() || colliderBlockMap_[c1] == c2 || !c1->GetGameObject()->ActiveSelf() || c1 == c2)
				continue;

			if (c1->IsColliding(c2))
			{
				colliderMap_[c1].emplace_back(c2);
				colliderMap_[c2].emplace_back(c1);
			}
		}
	}
}

ColliderManager::ColliderManager()
{
}

ColliderManager::~ColliderManager()
{
	ClearColliders();
}

void ColliderManager::ClearColliders()
{
	colliders2D_.clear();
	colliders_.clear();
	collider2DMap_.clear();
	colliderMap_.clear();
	colliderBlockMap_.clear();
}

void ColliderManager::AddCollider(Collider2D* c)
{
	colliders2D_.emplace_back(c);
	collider2DMap_.insert({ c, std::vector<Collider2D*>() });
}

void ColliderManager::AddCollider(Collider* c)
{
	colliders_.emplace_back(c);
	colliderMap_.insert({ c, std::vector<Collider*>() });
}

void ColliderManager::AddBlockage(const Collider* c1, const Collider* c2)
{
	colliderBlockMap_[c1] = c2;
	colliderBlockMap_[c2] = c1;
}

void ColliderManager::RemoveCollider(Collider* c)
{
	if (!colliderMap_.empty())
		colliderMap_.erase(c);
	if (!colliders_.empty())
		colliders_.erase(std::remove(colliders_.begin(), colliders_.end(), c), colliders_.end());
	if (!colliderBlockMap_.empty())
	{
		colliderBlockMap_.erase(c);
		for (auto& col : colliderBlockMap_)
			if (col.second == c)
				colliderBlockMap_.erase(col.first);
	}
}

void ColliderManager::Update(double dt)
{
		Update2D();
		Update3D();
}

std::vector<Collider2D*> ColliderManager::RetrieveCollisionData2D(const Collider2D* c1) const
{
	auto list = collider2DMap_.find(c1);
	return (list != collider2DMap_.end()) ? list->second : std::vector<Collider2D*>();
}

//std::vector<Collider2D*> ColliderManager::RetrieveCollisionDataIgnoringSingular2D(const Collider2D* c1,
//	const Collider2D* ignoringCollider) const
//{
//	//if (!c1) return std::vector<Collider2D*>();
//	//std::vector<Collider2D*> temp;
//	//for (const auto& c2 : _colliders2D)
//	//{
//	//	if (c1 == c2) continue;
//	//	if (c1 == ignoringCollider || c2 == ignoringCollider) continue;
//	//	if (c1->IsColliding(c2))
//	//	{
//	//		temp.push_back(c2);
//	//	}
//	//}
//	//return temp;
//}

std::vector<Collider*> ColliderManager::RetrieveCollisionData(const Collider* c1) const
{
	auto list = colliderMap_.find(c1);
	return (list != colliderMap_.end()) ? list->second : std::vector<Collider*>();
}


