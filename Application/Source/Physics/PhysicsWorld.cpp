#include "PhysicsWorld.h"

Collider2D* PhysicsWorld::AddCollider(Collider2D *collider)
{
    colliders_.emplace_back(collider);
    return collider;
}

void PhysicsWorld::Update(Timestep ts)
{
    
}
