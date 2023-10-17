#include "PhysicsWorld.h"

#include "Utils/Util.h"
#include "EC/Components/Collider2D.h"
#include "Physics/Bounds.h"

Collider2D *PhysicsWorld::AddCollider(Collider2D *collider)
{
    colliders_.emplace_back(collider);
    return collider;
}

void PhysicsWorld::Update(Timestep ts)
{
    
}

void PhysicsWorld::FixedUpdate(float fixedDeltaTime)
{
    for (uint32_t i = 0; i < colliders_.size(); ++i)
    {
        Collider2D* colliderX = colliders_[i];

        for (uint32_t j = i + 1; j < colliders_.size(); ++j)
        {
            Collider2D* colliderY = colliders_[j];
            if (colliderX->GetBounds().Intersects(colliderY->GetBounds()))
            {
                CC_TRACE("Collision");
            }
        }
    }
}
