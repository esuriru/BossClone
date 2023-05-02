#pragma once
#include "ECS/System.h"
#include "Physics/PhysicsSystem.h"

#include "Events/KeyEvent.h"
#include "Core/Timestep.h"
#include "Core/Core.h"

class PlayerSystem : public System 
{
public:
    auto Update(Timestep ts) -> void;

    Ref<PhysicsSystem> physicsSystem;
private:
};