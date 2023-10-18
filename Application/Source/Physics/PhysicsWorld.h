#pragma once
 
#include "Utils/Singleton.h"
#include "EC/Components/Collider2D.h"

#include <vector> 


class PhysicsWorld : public Utility::Singleton<PhysicsWorld>
{
public:
    PhysicsWorld() = default;
    uint32_t AddCollider(Collider2D* collider);
    void RemoveColliderAtIndex(uint32_t index);
    void RemoveCollider(Collider2D* collider);


    void Update(Timestep ts);
    void FixedUpdate(float fixedDeltaTime);

private:
    std::vector<Collider2D*> colliders_;   

};