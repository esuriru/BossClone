#pragma once
#include <unordered_map>
#include "Components/CircleCollider2D.h"
#include "Singleton.h"
#include "Components/Component.h"
#include "Components/BoxCollider2D.h"
#include <vector>
#include "Components/Collider2D.h"
#include "Components/Collider.h"

class ColliderManager : public Singleton<ColliderManager>
{
private:
	std::vector<Collider2D*> colliders2D_;
	std::unordered_map<const Collider2D*, std::vector<Collider2D*>> collider2DMap_;

	std::unordered_map<const Collider*, std::vector<Collider*>> colliderMap_;
	std::vector<Collider*> colliders_;

	std::unordered_map<const Collider*, const Collider*> colliderBlockMap_;

	void Update2D();
	void Update3D();

public:
	ColliderManager();
	virtual ~ColliderManager();

	void ClearColliders();
	void AddCollider(Collider2D* c);
	void AddCollider(Collider* c);

	void AddBlockage(const Collider* c1, const Collider* c2);

	void RemoveCollider(Collider* c);
	void Update(double dt);
	//std::vector<Collider2D*> RetrieveCollisionDataIgnoringSingular2D(const Collider2D* c1, const Collider2D* ignoringCollider) const;

	std::vector<Collider*> RetrieveCollisionData(const Collider* c1) const;
	std::vector<Collider2D*> RetrieveCollisionData2D(const Collider2D* c1) const;
	
};


