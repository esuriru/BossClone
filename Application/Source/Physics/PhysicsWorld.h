#pragma once
 
#include "Utils/Singleton.h"
#include "EC/Components/Collider2D.h"

#include <vector> 


class PhysicsWorld : Utility::Singleton<PhysicsWorld>
{
public:
    PhysicsWorld() = default;
    Collider2D* AddCollider(Collider2D* collider);

    void Update(Timestep ts);

private:
    std::vector<Collider2D*> colliders_;   

};