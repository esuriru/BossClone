#include "GameSystem.h"

#include "ECS/Coordinator.h"
#include "ECS/Component.h"

static Coordinator* coordinator = Coordinator::Instance();
auto MeleeWeaponSystem::Update(Timestep ts) -> void
{
    for (auto& e : entities)
    {
        auto& melee_weapon = coordinator->GetComponent<MeleeWeaponComponent>(e);

         
    } 
}