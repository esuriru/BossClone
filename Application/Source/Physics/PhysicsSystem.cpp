#include "PhysicsSystem.h"

#include "ECS/Coordinator.h"
#include "Game/Tilemap.h"

static Coordinator* coordinator = Coordinator::Instance();
auto PhysicsSystem::Update(Timestep ts) -> void
{
    for (auto& e : entities)
    {
        // NOTE - Maybe have another system that handles the tilemaps.
        auto& tilemap = coordinator->GetComponent<TilemapComponent>(e);
    }
}